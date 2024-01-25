//
// Created by haridev on 10/8/23.
//
#include <dlio_profiler/core/dlio_profiler_main.h>

void dlio_finalize() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore.dlio_finalize","");
  auto conf = dlio_profiler::Singleton<dlio_profiler::ConfigurationManager>::get_instance();
  if (conf->init_type == ProfileInitType::PROFILER_INIT_FUNCTION) {
    auto dlio_profiler = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI,
                                                      ProfileType::PROFILER_ANY);
    if (dlio_profiler != nullptr) {
      dlio_profiler->finalize();
      dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::finalize();
    }
  }
}

dlio_profiler::DLIOProfilerCore::DLIOProfilerCore(ProfilerStage stage, ProfileType type, const char *log_file,
                                                  const char *data_dirs, const int *process_id) :
             is_initialized(false), bind(false), include_metadata(false){
  conf = dlio_profiler::Singleton<dlio_profiler::ConfigurationManager>::get_instance();
  DLIO_PROFILER_LOGINFO("Loading DLIO Profiler with ProfilerStage %d ProfileType %d and process %d", stage, type, process_id);
  switch (type) {
    case ProfileType::PROFILER_ANY:
    case ProfileType::PROFILER_PRELOAD: {
      if (stage == ProfilerStage::PROFILER_INIT) {
        if (conf->init_type == ProfileInitType::PROFILER_INIT_LD_PRELOAD) {
          initialize(true, log_file, data_dirs, process_id);
        }
        DLIO_PROFILER_LOGINFO("Preloading DLIO Profiler with log_file %s data_dir %s and process %d",
                              this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
      }
      break;
    }
    case ProfileType::PROFILER_PY_APP:
    case ProfileType::PROFILER_C_APP:
    case ProfileType::PROFILER_CPP_APP: {
      bool bind = false;
      if (stage == ProfilerStage::PROFILER_INIT && conf->init_type == ProfileInitType::PROFILER_INIT_FUNCTION) {
          bind = true;
      }
      initialize(bind, log_file, data_dirs, process_id);
      DLIO_PROFILER_LOGINFO("App Initializing DLIO Profiler with log_file %s data_dir %s and process %d",
                            this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
      break;
    }
    default: {  // GCOVR_EXCL_START
      DLIO_PROFILER_LOGERROR(UNKNOWN_PROFILER_TYPE.message, type);
      throw std::runtime_error(UNKNOWN_PROFILER_TYPE.code);
    }  // GCOVR_EXCL_STOP
  }
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::DLIOProfilerCore type %d",type);
}

void dlio_profiler::DLIOProfilerCore::log(ConstEventType event_name, ConstEventType category,
                                          TimeResolution start_time, TimeResolution duration,
                                          std::unordered_map<std::string, std::any> *metadata) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::log","");
  if (this->is_initialized && conf->enable) {
    if (logger != nullptr) {
      logger->log(event_name, category, start_time, duration, metadata);
    } else  {
      DLIO_PROFILER_LOGERROR("DLIOProfilerCore::log logger not initialized","");
    }
  }
}

bool dlio_profiler::DLIOProfilerCore::finalize() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::finalize","");
  if (this->is_initialized && conf->enable) {
    DLIO_PROFILER_LOGINFO("Calling finalize on pid %d", this->process_id);
    auto trie = dlio_profiler::Singleton<Trie>::get_instance();
    if (trie != nullptr) {
      DLIO_PROFILER_LOGINFO("Release Prefix Tree","");
      trie->finalize();
      dlio_profiler::Singleton<Trie>::finalize();
    }
    if (bind && conf->io) {
      DLIO_PROFILER_LOGINFO("Release I/O bindings","");
      free_bindings();
      auto posix_instance = brahma::POSIXDLIOProfiler::get_instance();
      if (posix_instance != nullptr) {
        posix_instance->finalize();
      }
      auto stdio_instance = brahma::STDIODLIOProfiler::get_instance();
      if (stdio_instance != nullptr) {
        stdio_instance->finalize();
      }
    }
    if (logger != nullptr) {
      logger->finalize();
      dlio_profiler::Singleton<DLIOLogger>::finalize();
    }
    this->is_initialized = false;
    return true;
  }
  return false;
}

void
dlio_profiler::DLIOProfilerCore::initialize(bool _bind, const char *_log_file, const char *_data_dirs,
                                             const int *_process_id) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::initialize","");
  if (conf->bind_signals) set_signal();
  if (!is_initialized) {
    this->bind = _bind;
    include_metadata = conf->metadata;
    logger = dlio_profiler::Singleton<DLIOLogger>::get_instance();
    if (conf->enable) {
      DLIO_PROFILER_LOGDEBUG("DLIO Profiler enabled", "");
      if (_process_id == nullptr || *_process_id == -1) {
        this->process_id = dlp_getpid();
      } else {
        this->process_id = *_process_id;
      }
      DLIO_PROFILER_LOGDEBUG("Setting process_id to %d", this->process_id);
      if (_log_file == nullptr) {
        char cmd[128];
        sprintf(cmd, "/proc/%lu/cmdline", dlp_getpid());
        int fd = dlp_open(cmd, O_RDONLY);
        std::string exec_name = "DEFAULT";

        if (fd != -1) {
          char exec_file_name[DLP_PATH_MAX];
          ssize_t read_bytes = dlp_read(fd, exec_file_name, DLP_PATH_MAX);
          dlp_close(fd);
          ssize_t index = 0;
          while (index < read_bytes - 1) {
            if (exec_file_name[index] == '\0') {
              exec_file_name[index] = SEPARATOR;
            }
            index++;
          }
          DLIO_PROFILER_LOGDEBUG("Exec command line %s", exec_file_name);
          auto items = split(exec_file_name, SEPARATOR);
          for (const auto item : items) {
            if (strstr(item.c_str(), "python") == nullptr) {
              exec_name = basename(item.c_str());
              break;
            }
          }
        }
        DLIO_PROFILER_LOGINFO("Extracted process_name %s", exec_name.c_str());
        if (!conf->log_file.empty()) {
            this->log_file = std::string(conf->log_file) + "-" + exec_name + "-" + std::to_string(this->process_id) + ".pfw" ;
        } else {  // GCOV_EXCL_START
          DLIO_PROFILER_LOGERROR(UNDEFINED_LOG_FILE.message, "");
          throw std::runtime_error(UNDEFINED_LOG_FILE.code);
        } // GCOV_EXCL_STOP
      } else {
        this->log_file = _log_file;
      }
      DLIO_PROFILER_LOGDEBUG("Setting log file to %s", this->log_file.c_str());
      logger->update_log_file(this->log_file, this->process_id);
      if (bind) {
        if (conf->io) {
          auto trie = dlio_profiler::Singleton<Trie>::get_instance();
          const char* ignore_extensions[2] = {"pfw", "py"};
          const char* ignore_prefix[8] = {"/pipe", "/socket", "/proc",
                                          "/sys", "/collab",
                                          "anon_inode", "socket", "/var/tmp"};
          for(const char* folder: ignore_prefix) {
            trie->exclude(folder, strlen(folder));
          }
          for(const char* ext: ignore_extensions) {
            trie->exclude_reverse(ext, strlen(ext));
          }
          if (!conf->trace_all_files) {
            if (_data_dirs == nullptr) {
              if (!conf->data_dirs.empty()) {
                this->data_dirs = conf->data_dirs;
              } else {  // GCOV_EXCL_START
                DLIO_PROFILER_LOGERROR(UNDEFINED_DATA_DIR.message, "");
                throw std::runtime_error(UNDEFINED_DATA_DIR.code);
              }  // GCOV_EXCL_STOP
            } else {
              this->data_dirs = _data_dirs;
              if (!conf->data_dirs.empty()) {
                this->data_dirs += ":" + conf->data_dirs;
              }
            }
            DLIO_PROFILER_LOGDEBUG("Setting data_dirs to %s",
                                   this->data_dirs.c_str());
          } else {
            DLIO_PROFILER_LOGDEBUG("Ignoring data_dirs as tracing all files","");
          }
          brahma_gotcha_wrap("dlio_profiler", conf->gotcha_priority);
          if (!conf->trace_all_files) {
            auto paths = split(this->data_dirs, DLIO_PROFILER_DATA_DIR_DELIMITER);
            for (const auto &path : paths) {
              DLIO_PROFILER_LOGDEBUG("Profiler will trace %s\n", path.c_str());
              trie->include(path.c_str(), path.size());
            }
          }
          if (conf->posix) {
            brahma::POSIXDLIOProfiler::get_instance(conf->trace_all_files);
          }
          if (conf->stdio) {
            brahma::STDIODLIOProfiler::get_instance(conf->trace_all_files);
          }
        }
      }
    }
    is_initialized = true;
  }
}

TimeResolution dlio_profiler::DLIOProfilerCore::get_time() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::get_time","");
  if (this->is_initialized && conf->enable && logger != nullptr) {
    return logger->get_time();
  } else  {
    DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::get_time logger not initialized","");
  }
  return -1;
}