//
// Created by haridev on 10/8/23.
//
#include <dftracer/core/dftracer_main.h>
template <>
std::shared_ptr<dftracer::DFTracerCore>
    dftracer::Singleton<dftracer::DFTracerCore>::instance = nullptr;
template <>
bool dftracer::Singleton<dftracer::DFTracerCore>::stop_creating_instances =
    false;
void dft_finalize() {
  DFTRACER_LOGDEBUG("DFTracerCore.dft_finalize", "");
  auto conf =
      dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
  if (conf->init_type == ProfileInitType::PROFILER_INIT_FUNCTION) {
    auto dftracer = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI,
                                            ProfileType::PROFILER_ANY);
    if (dftracer != nullptr) {
      dftracer->finalize();
      dftracer::Singleton<dftracer::DFTracerCore>::finalize();
    }
  }
}

dftracer::DFTracerCore::DFTracerCore(ProfilerStage stage, ProfileType type,
                                     const char *log_file,
                                     const char *data_dirs,
                                     const int *process_id)
    : is_initialized(false),
      bind(false),
      log_file_suffix(),
      include_metadata(false) {
  conf = dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
  DFTRACER_LOGINFO(
      "Loading DFTracer with ProfilerStage %d ProfileType %d and process "
      "%d",
      stage, type, process_id);
  switch (type) {
    case ProfileType::PROFILER_ANY:
    case ProfileType::PROFILER_PRELOAD: {
      if (stage == ProfilerStage::PROFILER_INIT) {
        log_file_suffix = "preload";
        if (conf->init_type == ProfileInitType::PROFILER_INIT_LD_PRELOAD) {
          initialize(true, log_file, data_dirs, process_id);
        }
        DFTRACER_LOGINFO(
            "Preloading DFTracer with log_file %s data_dir %s and process "
            "%d",
            this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
      }
      break;
    }
    case ProfileType::PROFILER_PY_APP:
    case ProfileType::PROFILER_C_APP:
    case ProfileType::PROFILER_CPP_APP: {
      log_file_suffix = "app";
      bool bind = false;
      if (stage == ProfilerStage::PROFILER_INIT &&
          conf->init_type == ProfileInitType::PROFILER_INIT_FUNCTION) {
        bind = true;
      }
      initialize(bind, log_file, data_dirs, process_id);
      DFTRACER_LOGINFO(
          "App Initializing DFTracer with log_file %s data_dir %s and "
          "process %d",
          this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
      break;
    }
    default: {  // GCOVR_EXCL_START
      DFTRACER_LOGERROR(UNKNOWN_PROFILER_TYPE.message, type);
      throw std::runtime_error(UNKNOWN_PROFILER_TYPE.code);
    }  // GCOVR_EXCL_STOP
  }
  DFTRACER_LOGDEBUG("DFTracerCore::DFTracerCore type %d", type);
}

void dftracer::DFTracerCore::log(
    ConstEventType event_name, ConstEventType category,
    TimeResolution start_time, TimeResolution duration,
    std::unordered_map<std::string, std::any> *metadata) {
  DFTRACER_LOGDEBUG("DFTracerCore::log", "");
  if (this->is_initialized && conf->enable) {
    if (logger != nullptr) {
      logger->log(event_name, category, start_time, duration, metadata);
    } else {
      DFTRACER_LOGERROR("DFTracerCore::log logger not initialized", "");
    }
  }
}

bool dftracer::DFTracerCore::finalize() {
  DFTRACER_LOGDEBUG("DFTracerCore::finalize", "");
  if (this->is_initialized && conf->enable) {
    DFTRACER_LOGINFO("Calling finalize on pid %d", this->process_id);
    auto trie = dftracer::Singleton<Trie>::get_instance();
    if (trie != nullptr) {
      DFTRACER_LOGINFO("Release Prefix Tree", "");
      trie->finalize();
      dftracer::Singleton<Trie>::finalize();
    }
    if (bind && conf->io) {
      DFTRACER_LOGINFO("Release I/O bindings", "");
      brahma_free_bindings();
      auto posix_instance = brahma::POSIXDFTracer::get_instance();
      if (posix_instance != nullptr) {
        posix_instance->finalize();
      }
      auto stdio_instance = brahma::STDIODFTracer::get_instance();
      if (stdio_instance != nullptr) {
        stdio_instance->finalize();
      }
    }
    if (logger != nullptr) {
      logger->finalize();
      dftracer::Singleton<DFTLogger>::finalize();
    }
    this->is_initialized = false;
    return true;
  }
  return false;
}

void dftracer::DFTracerCore::initialize(bool _bind, const char *_log_file,
                                        const char *_data_dirs,
                                        const int *_process_id) {
  DFTRACER_LOGDEBUG("DFTracerCore::initialize", "");
  if (conf->bind_signals) set_signal();
  if (!is_initialized) {
    this->bind = _bind;
    include_metadata = conf->metadata;
    logger = dftracer::Singleton<DFTLogger>::get_instance();
    if (conf->enable) {
      DFTRACER_LOGDEBUG("DFTracer enabled", "");
      if (_process_id == nullptr || *_process_id == -1) {
        this->process_id = df_getpid();
      } else {
        this->process_id = *_process_id;
      }
      DFTRACER_LOGDEBUG("Setting process_id to %d", this->process_id);
      if (_log_file == nullptr) {
        char cmd[128];
        sprintf(cmd, "/proc/%lu/cmdline", df_getpid());
        int fd = df_open(cmd, O_RDONLY);
        std::string exec_name = "DEFAULT";

        if (fd != -1) {
          char exec_file_name[DFT_PATH_MAX];
          ssize_t read_bytes = df_read(fd, exec_file_name, DFT_PATH_MAX);
          df_close(fd);
          ssize_t index = 0;
          while (index < read_bytes - 1) {
            if (exec_file_name[index] == '\0') {
              exec_file_name[index] = SEPARATOR;
            }
            index++;
          }
          DFTRACER_LOGDEBUG("Exec command line %s", exec_file_name);
          auto items = split(exec_file_name, SEPARATOR);
          for (const auto &item : items) {
            if (strstr(item.c_str(), "python") == nullptr) {
              exec_name = basename(item.c_str());
              break;
            }
          }
        }
        DFTRACER_LOGINFO("Extracted process_name %s", exec_name.c_str());
        if (!conf->log_file.empty()) {
          this->log_file = std::string(conf->log_file) + "-" + exec_name + "-" +
                           std::to_string(this->process_id) + "-" +
                           log_file_suffix + ".pfw";
        } else {  // GCOV_EXCL_START
          DFTRACER_LOGERROR(UNDEFINED_LOG_FILE.message, "");
          throw std::runtime_error(UNDEFINED_LOG_FILE.code);
        }  // GCOV_EXCL_STOP
      } else {
        this->log_file = _log_file;
      }
      DFTRACER_LOGDEBUG("Setting log file to %s", this->log_file.c_str());
      logger->update_log_file(this->log_file, this->process_id);
      if (bind) {
        if (conf->io) {
          auto trie = dftracer::Singleton<Trie>::get_instance();
          const char *ignore_extensions[3] = {".pfw", ".py",".pfw.gz"};
          const char *ignore_prefix[8] = {"/pipe",  "/socket", "/proc",
                                          "/sys",   "/collab", "anon_inode",
                                          "socket", "/var/tmp"};
          for (const char *folder : ignore_prefix) {
            trie->exclude(folder, strlen(folder));
          }
          for (const char *ext : ignore_extensions) {
            trie->exclude_reverse(ext, strlen(ext));
          }
          if (!conf->trace_all_files) {
            if (_data_dirs == nullptr) {
              if (!conf->data_dirs.empty()) {
                this->data_dirs = conf->data_dirs;
              } else {  // GCOV_EXCL_START
                DFTRACER_LOGERROR(UNDEFINED_DATA_DIR.message, "");
                throw std::runtime_error(UNDEFINED_DATA_DIR.code);
              }  // GCOV_EXCL_STOP
            } else {
              this->data_dirs = _data_dirs;
              if (!conf->data_dirs.empty()) {
                this->data_dirs += ":" + conf->data_dirs;
              }
            }
            DFTRACER_LOGDEBUG("Setting data_dirs to %s",
                              this->data_dirs.c_str());
          } else {
            DFTRACER_LOGDEBUG("Ignoring data_dirs as tracing all files", "");
          }
          brahma_gotcha_wrap("dftracer", conf->gotcha_priority);
          if (!conf->trace_all_files) {
            auto paths = split(this->data_dirs, DFTRACER_DATA_DIR_DELIMITER);
            for (const auto &path : paths) {
              DFTRACER_LOGDEBUG("Profiler will trace %s\n", path.c_str());
              trie->include(path.c_str(), path.size());
            }
          }
          if (conf->posix) {
            brahma::POSIXDFTracer::get_instance(conf->trace_all_files);
          }
          if (conf->stdio) {
            brahma::STDIODFTracer::get_instance(conf->trace_all_files);
          }
        }
      }
    }
    is_initialized = true;
  }
}

TimeResolution dftracer::DFTracerCore::get_time() {
  DFTRACER_LOGDEBUG("DFTracerCore::get_time", "");
  if (this->is_initialized && conf->enable && logger != nullptr) {
    return logger->get_time();
  } else {
    DFTRACER_LOGDEBUG("DFTracerCore::get_time logger not initialized", "");
  }
  return -1;
}
