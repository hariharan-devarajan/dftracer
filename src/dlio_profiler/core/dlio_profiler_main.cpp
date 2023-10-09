//
// Created by haridev on 10/8/23.
//
#include <dlio_profiler/core/dlio_profiler_main.h>


#include <dlio_profiler/brahma/posix.h>
#include <dlio_profiler/brahma/stdio.h>
#include <dlio_profiler/dlio_logger.h>

dlio_profiler::DLIOProfilerCore::DLIOProfilerCore(ProfilerStage stage, ProfileType type, const char *log_file,
                                                  const char *data_dirs, const int *process_id): is_enabled(
        false), gotcha_priority(1), logger_level(cpplogger::LoggerType::LOG_ERROR), log_file(), data_dirs(), is_initialized(false), bind(false) {
  const char* user_init_type = getenv(DLIO_PROFILER_INIT);
  switch (type) {
    case ProfileType::PROFILER_PRELOAD: {
      if (stage == ProfilerStage::PROFILER_INIT) {
        if (user_init_type != nullptr && strcmp(user_init_type, "PRELOAD") == 0) {
          initlialize(true, true, log_file, data_dirs, process_id);
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
        initlialize(true, bind, log_file, data_dirs, process_id);
        DLIO_PROFILER_LOGINFO("Initializing DLIO Profiler with log_file %s data_dir %s and process %d",
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

void dlio_profiler::DLIOProfilerCore::log(const char *event_name, const char *category, TimeResolution start_time,
                                          TimeResolution duration,
                                          std::unordered_map<std::string, std::any> &metadata)  {
  if (this->is_initialized && is_enabled) {
    dlio_profiler::Singleton<DLIOLogger>::get_instance(false)->
            log(event_name, category, start_time, duration, metadata);
  }
}

bool dlio_profiler::DLIOProfilerCore::finalize() {
  if (this->is_initialized && is_enabled) {
    DLIO_PROFILER_LOGINFO("Calling finalize on pid %d", this->process_id);
    dlio_profiler::Singleton<DLIOLogger>::get_instance(false)->finalize();
    if (bind) {
      free_bindings();
    }
    this->is_initialized = false;
    return true;
  }
  return false;
}

void
dlio_profiler::DLIOProfilerCore::initlialize(bool is_init, bool _bind, const char *_log_file, const char *_data_dirs,
                                             const int *_process_id)  {
  this->bind = _bind;
  bind_signals();
  if (is_init) {
    char *dlio_profiler_log_level = getenv(DLIO_PROFILER_LOG_LEVEL);
    if (dlio_profiler_log_level == nullptr) {  // GCOV_EXCL_START
      logger_level = cpplogger::LoggerType::LOG_ERROR;
    } else {
      if (strcmp(dlio_profiler_log_level, "ERROR") == 0) {
        logger_level = cpplogger::LoggerType::LOG_ERROR;
      } else if (strcmp(dlio_profiler_log_level, "INFO") == 0) {
        logger_level = cpplogger::LoggerType::LOG_INFO;
      } else if (strcmp(dlio_profiler_log_level, "DEBUG") == 0) {
        logger_level = cpplogger::LoggerType::LOG_WARN;
      }
    }  // GCOV_EXCL_STOP
    DLIO_PROFILER_LOGGER->level = logger_level;
    DLIO_PROFILER_LOGINFO("Enabling logging level %d", logger_level);

    char *dlio_profiler_enable = getenv(DLIO_PROFILER_ENABLE);
    if (dlio_profiler_enable != nullptr && strcmp(dlio_profiler_enable, "1") == 0) {
      is_enabled = true;
    }
    if (is_enabled) {
      DLIO_PROFILER_LOGINFO("DLIO Profiler enabled", "");
      char *dlio_profiler_priority_str = getenv(DLIO_PROFILER_GOTCHA_PRIORITY);
      if (dlio_profiler_priority_str != nullptr) {
        gotcha_priority = atoi(dlio_profiler_priority_str); // GCOV_EXCL_LINE
      }
      if (_process_id == nullptr || *_process_id == -1) {
        this->process_id = getpid();
      } else {
        this->process_id = *_process_id;
      }
      DLIO_PROFILER_LOGINFO("Setting process_id to %d", this->process_id);
      if (_log_file == nullptr) {
        char *dlio_profiler_log = getenv(DLIO_PROFILER_LOG_FILE);
        if (dlio_profiler_log != nullptr) {
          this->log_file = std::string(dlio_profiler_log) + "-" + std::to_string(this->process_id) + ".pfw";
        } else {  // GCOV_EXCL_START
          DLIO_PROFILER_LOGERROR(UNDEFINED_LOG_FILE.message, "");
          throw std::runtime_error(UNDEFINED_LOG_FILE.code);
        } // GCOV_EXCL_STOP
      } else {
        this->log_file = _log_file;
      }
      DLIO_PROFILER_LOGINFO("Setting log file to %s", this->log_file.c_str());
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

      DLIO_PROFILER_LOGINFO("Setting data_dirs to %s", this->data_dirs.c_str());


      dlio_profiler::Singleton<DLIOLogger>::get_instance()->update_log_file(this->log_file, this->process_id);
      if (bind) {
        brahma_gotcha_wrap("dlio_profiler", this->gotcha_priority);
        auto posix_instance = brahma::POSIXDLIOProfiler::get_instance();
        auto stdio_instance = brahma::STDIODLIOProfiler::get_instance();
        auto paths = split(this->data_dirs, ':');
        posix_instance->untrace(this->log_file.c_str());
        stdio_instance->untrace(this->log_file.c_str());
        for (const auto &path:paths) {
          DLIO_PROFILER_LOGINFO("Profiler will trace %s\n", path.c_str());
          posix_instance->trace(path.c_str());
          stdio_instance->trace(path.c_str());
        }
      }
    }
    is_initialized = true;
  }
}

TimeResolution dlio_profiler::DLIOProfilerCore::get_time()  {
  if (this->is_initialized && is_enabled) {
    return dlio_profiler::Singleton<DLIOLogger>::get_instance(false)->get_time();
  }
  return -1;
}