//
// Created by haridev on 10/8/23.
//
#include <dlio_profiler/core/dlio_profiler_main.h>



void dlio_finalize() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore.dlio_finalize","");
  const char *user_init_type = getenv(DLIO_PROFILER_INIT);
  if (user_init_type == nullptr || strcmp(user_init_type, "FUNCTION") == 0) {
    auto dlio_profiler = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI,
                                                      ProfileType::PROFILER_ANY);
    if (dlio_profiler != nullptr) {
      dlio_profiler->finalize();
      dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::finalize();
    }
  }
}

dlio_profiler::DLIOProfilerCore::DLIOProfilerCore(ProfilerStage stage, ProfileType type, const char *log_file,
                                                  const char *data_dirs, const int *process_id) : is_enabled(
        false), gotcha_priority(1), logger_level(cpplogger::LoggerType::LOG_ERROR), log_file(), data_dirs(),
        is_initialized(false), bind(false), enable_io(false), enable_stdio(false), enable_posix(false),trace_all_files(false),
                                                                                                  include_metadata(false){
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::DLIOProfilerCore","");
  const char *user_init_type = getenv(DLIO_PROFILER_INIT);
  switch (type) {
    case ProfileType::PROFILER_ANY:
    case ProfileType::PROFILER_PRELOAD: {
      if (stage == ProfilerStage::PROFILER_INIT) {
        if (user_init_type != nullptr && strcmp(user_init_type, "PRELOAD") == 0) {
          initlialize(true, log_file, data_dirs, process_id);
        }
        DLIO_PROFILER_LOGINFO("Preloading DLIO Profiler with log_file %s data_dir %s and process %d",
                              this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
      }
      break;
    }
    case ProfileType::PROFILER_PY_APP:
    case ProfileType::PROFILER_C_APP:
    case ProfileType::PROFILER_CPP_APP: {
      if (stage == ProfilerStage::PROFILER_INIT) {
        bool bind = false;
        if (user_init_type == nullptr || strcmp(user_init_type, "FUNCTION") == 0) {
          bind = true;
        }
        initlialize(bind, log_file, data_dirs, process_id);
        DLIO_PROFILER_LOGINFO("App Initializing DLIO Profiler with log_file %s data_dir %s and process %d",
                              this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
      }
      break;
    }
    default: {  // GCOVR_EXCL_START
      DLIO_PROFILER_LOGERROR(UNKNOWN_PROFILER_TYPE.message, type);
      throw std::runtime_error(UNKNOWN_PROFILER_TYPE.code);
    }  // GCOVR_EXCL_STOP
  }
}

void dlio_profiler::DLIOProfilerCore::log(ConstEventType event_name, ConstEventType category,
                                          TimeResolution start_time, TimeResolution duration,
                                          std::unordered_map<std::string, std::any> *metadata) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::log","");
  if (this->is_initialized && is_enabled) {
    auto logger = dlio_profiler::Singleton<DLIOLogger>::get_instance();
    if (logger != nullptr) {
      logger->log(event_name, category, start_time, duration, metadata);
    }
  }
}

bool dlio_profiler::DLIOProfilerCore::finalize() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::finalize","");
  if (this->is_initialized && is_enabled) {
    DLIO_PROFILER_LOGINFO("Calling finalize on pid %d", this->process_id);
    auto trie = dlio_profiler::Singleton<Trie>::get_instance();
    if (trie != nullptr) {
      DLIO_PROFILER_LOGINFO("Release Prefix Tree","");
      trie->finalize();
      dlio_profiler::Singleton<Trie>::finalize();
    }
    if (bind && enable_io) {
      auto posix_instance = brahma::POSIXDLIOProfiler::get_instance();
      if (posix_instance != nullptr) {
        posix_instance->finalize();
      }
      auto stdio_instance = brahma::STDIODLIOProfiler::get_instance();
      if (stdio_instance != nullptr) {
        stdio_instance->finalize();
      }
      DLIO_PROFILER_LOGINFO("Release I/O bindings","");
      free_bindings();
    }
    auto logger = dlio_profiler::Singleton<DLIOLogger>::get_instance();
    if (logger != nullptr) {
      logger->finalize();
      dlio_profiler::Singleton<DLIOLogger>::finalize();
    }
    this->is_initialized = false;
    this->is_enabled = false;
    return true;
  }
  return false;
}

void
dlio_profiler::DLIOProfilerCore::initlialize(bool _bind, const char *_log_file, const char *_data_dirs,
                                             const int *_process_id) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::initlialize","");
  char *dlio_profiler_signal = getenv(DLIO_PROFILER_BIND_SIGNALS);
  if (dlio_profiler_signal == nullptr || strcmp(dlio_profiler_signal, "1") == 0) {
    set_signal();
  }
  if (!is_initialized) {
    this->bind = _bind;
    char *dlio_profiler_meta = getenv(DLIO_PROFILER_INC_METADATA);
    if (dlio_profiler_meta != nullptr && strcmp(dlio_profiler_meta, "1") == 0) {
      include_metadata = true;
    }
    auto logger = dlio_profiler::Singleton<DLIOLogger>::get_instance();
    char *dlio_profiler_log_level = getenv(DLIO_PROFILER_LOG_LEVEL);
    if (dlio_profiler_log_level == nullptr) {  // GCOV_EXCL_START
      logger_level = cpplogger::LoggerType::LOG_ERROR;
    } else {
      if (strcmp(dlio_profiler_log_level, "ERROR") == 0) {
        logger_level = cpplogger::LoggerType::LOG_ERROR;
      } else if (strcmp(dlio_profiler_log_level, "INFO") == 0) {
        logger_level = cpplogger::LoggerType::LOG_INFO;
      } else if (strcmp(dlio_profiler_log_level, "DEBUG") == 0) {
        logger_level = cpplogger::LoggerType::LOG_DEBUG;
      } else if (strcmp(dlio_profiler_log_level, "WARN") == 0) {
        logger_level = cpplogger::LoggerType::LOG_WARN;
      }
    }  // GCOV_EXCL_STOP
    DLIO_PROFILER_LOGGER->level = logger_level;
    DLIO_PROFILER_LOGDEBUG("Enabling logging level %d", logger_level);

    char *dlio_profiler_enable = getenv(DLIO_PROFILER_ENABLE);
    if (dlio_profiler_enable != nullptr && strcmp(dlio_profiler_enable, "1") == 0) {
      is_enabled = true;
    }
    if (is_enabled) {
      DLIO_PROFILER_LOGDEBUG("DLIO Profiler enabled", "");
      char *dlio_profiler_priority_str = getenv(DLIO_PROFILER_GOTCHA_PRIORITY);
      if (dlio_profiler_priority_str != nullptr) {
        gotcha_priority = atoi(dlio_profiler_priority_str); // GCOV_EXCL_LINE
      }
      if (_process_id == nullptr || *_process_id == -1) {
        this->process_id = dlp_getpid();
      } else {
        this->process_id = *_process_id;
      }
      DLIO_PROFILER_LOGDEBUG("Setting process_id to %d and thread id to %d", this->process_id);
      if (_log_file == nullptr) {
        char *dlio_profiler_log = getenv(DLIO_PROFILER_LOG_FILE);
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
        if (dlio_profiler_log != nullptr) {
            this->log_file = std::string(dlio_profiler_log) + "-" + exec_name + "-" + std::to_string(this->process_id) + ".pfw" ;
        } else {  // GCOV_EXCL_START
          DLIO_PROFILER_LOGERROR(UNDEFINED_LOG_FILE.message, "");
          throw std::runtime_error(UNDEFINED_LOG_FILE.code);
        } // GCOV_EXCL_STOP
      } else {
        this->log_file = _log_file;
      }
      DLIO_PROFILER_LOGDEBUG("Setting log file to %s", this->log_file.c_str());
      if (_data_dirs == nullptr) {
        char *dlio_profiler_data_dirs = getenv(DLIO_PROFILER_DATA_DIR);
        if (dlio_profiler_data_dirs != nullptr) {
          this->data_dirs = dlio_profiler_data_dirs;
        } else { // GCOV_EXCL_START
          DLIO_PROFILER_LOGERROR(UNDEFINED_DATA_DIR.message, "");
          throw std::runtime_error(UNDEFINED_DATA_DIR.code);
        } // GCOV_EXCL_STOP
      } else {
        this->data_dirs = _data_dirs;
      }
      DLIO_PROFILER_LOGDEBUG("Setting data_dirs to %s", this->data_dirs.c_str());
      logger->update_log_file(this->log_file, this->process_id);
      if (bind) {
        auto trie = dlio_profiler::Singleton<Trie>::get_instance();
        const char* ignore_extensions[2] = {"pfw", "py"};
        const char* ignore_prefix[3] = {"/pipe", "/socket", "/proc"};
        for(const char* folder: ignore_prefix) {
          trie->exclude(folder, strlen(folder));
        }
        for(const char* ext: ignore_extensions) {
          trie->exclude_reverse(ext, strlen(ext));
        }

        char *trace_all_str = getenv(DLIO_PROFILER_TRACE_ALL_FILES);
        if (trace_all_str != nullptr && strcmp(trace_all_str, "1") != 0) {
          trace_all_files = true;
        }
        char *disable_io = getenv(DLIO_PROFILER_DISABLE_IO);
        char *disable_posix = getenv(DLIO_PROFILER_DISABLE_POSIX);
        char *disable_stdio = getenv(DLIO_PROFILER_DISABLE_STDIO);
        if (disable_io == nullptr || strcmp(disable_io, "1") != 0) {
          enable_io = true;

          auto paths = split(this->data_dirs, ':');
          brahma_gotcha_wrap("dlio_profiler", this->gotcha_priority);
          auto cwd = fs::current_path();
          for (const auto &path:paths) {
            DLIO_PROFILER_LOGDEBUG("Profiler will trace %s\n", path.c_str());
            trie->include(path.c_str(), path.size());
            auto relative_dir = fs::relative(path, cwd).generic_string();
            DLIO_PROFILER_LOGDEBUG("Profiler will trace %s\n", relative_dir.c_str());
            trie->include(relative_dir.c_str(), relative_dir.size());
            relative_dir = "./" + relative_dir;
            DLIO_PROFILER_LOGDEBUG("Profiler will trace %s\n", relative_dir.c_str());
            trie->include(relative_dir.c_str(), relative_dir.size());
            DLIO_PROFILER_LOGDEBUG("Profiler will trace %s\n", relative_dir.c_str());
          }
          if (disable_posix == nullptr || strcmp(disable_posix, "1") != 0) {
            enable_posix = true;
            brahma::POSIXDLIOProfiler::get_instance(this->trace_all_files);
          }
          if (disable_stdio == nullptr || strcmp(disable_stdio, "1") != 0) {
            enable_stdio = true;
            brahma::STDIODLIOProfiler::get_instance(this->trace_all_files);
          }
        }
      }
    }
    is_initialized = true;
  }
}

TimeResolution dlio_profiler::DLIOProfilerCore::get_time() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore::get_time","");
  auto logger = dlio_profiler::Singleton<DLIOLogger>::get_instance();
  if (this->is_initialized && is_enabled && logger != nullptr) {
    return logger->get_time();
  }
  return -1;
}