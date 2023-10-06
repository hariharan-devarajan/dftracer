//
// Created by haridev on 10/5/23.
//

#ifndef DLIO_PROFILER_DLIO_PROFILER_MAIN_H
#define DLIO_PROFILER_DLIO_PROFILER_MAIN_H
#include <cstring>
#include <thread>
#include <stdexcept>

#include <cpp-logger/logger.h>
#include <dlio_profiler/core/constants.h>
#include <dlio_profiler/macro.h>
#include <dlio_profiler/brahma/posix.h>
#include <dlio_profiler/brahma/stdio.h>
#include <dlio_profiler/dlio_logger.h>
#include <brahma/brahma.h>
#include <execinfo.h>
#include "singleton.h"

static void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

namespace dlio_profiler {
    class DLIOProfiler {
    private:
        bool is_enabled;
        int gotcha_priority;
        cpplogger::LoggerType logger_level;
        std::string log_file;
        std::string data_dirs;
        int process_id;
        bool is_init;
    public:
        DLIOProfiler(bool is_init, const char *log_file = nullptr, const char *data_dirs = nullptr, const int *process_id = nullptr) : is_enabled(
                false), gotcha_priority(1), logger_level(cpplogger::LoggerType::LOG_ERROR), log_file(), data_dirs(
                ), is_init(is_init) {
          signal(SIGSEGV, handler);
          if (this->is_init) {
            char *dlio_profiler_log_level = getenv(DLIO_PROFILER_LOG_LEVEL);
            if (dlio_profiler_log_level == nullptr) {
              logger_level = cpplogger::LoggerType::LOG_ERROR;
            } else {
              if (strcmp(dlio_profiler_log_level, "ERROR") == 0) {
                logger_level = cpplogger::LoggerType::LOG_ERROR;
              } else if (strcmp(dlio_profiler_log_level, "INFO") == 0) {
                logger_level = cpplogger::LoggerType::LOG_INFO;
              } else if (strcmp(dlio_profiler_log_level, "DEBUG") == 0) {
                logger_level = cpplogger::LoggerType::LOG_WARN;
              }
            }
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
                gotcha_priority = atoi(dlio_profiler_priority_str);
              }

              if (log_file == nullptr) {
                char *dlio_profiler_log = getenv(DLIO_PROFILER_LOG_FILE);
                if (dlio_profiler_log != nullptr) {
                  this->log_file = dlio_profiler_log;
                } else {
                  const char *message = "log_file not defined. Please define env variable DLIO_PROFILER_LOG_FILE";
                  DLIO_PROFILER_LOGERROR(message, "");
                  throw std::runtime_error(message);
                }
              } else {
                this->log_file = log_file;
              }
              DLIO_PROFILER_LOGINFO("Setting log file to %s", this->log_file.c_str());
              if (data_dirs == nullptr) {
                char *dlio_profiler_data_dirs = getenv(DLIO_PROFILER_DATA_DIR);
                if (dlio_profiler_data_dirs != nullptr) {
                  this->data_dirs = dlio_profiler_data_dirs;
                } else {
                  const char *message = "data_dirs not defined. Please define env variable DLIO_PROFILER_DATA_DIR";
                  DLIO_PROFILER_LOGERROR(message, "");
                  throw std::runtime_error(message);
                }
              } else {
                this->data_dirs = data_dirs;
              }

              DLIO_PROFILER_LOGINFO("Setting data_dirs to %s", this->data_dirs.c_str());
              if (process_id == nullptr) {
                this->process_id = getpid();
              } else {
                this->process_id = *process_id;
              }
              DLIO_PROFILER_LOGINFO("Setting process_id to %s", this->process_id);

              dlio_profiler::Singleton<DLIOLogger>::get_instance()->update_log_file(this->log_file, this->process_id);
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
              size_t thread_hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
              DLIO_PROFILER_LOGINFO("Running DLIO Profiler on thread %ld and pid %ld", thread_hash, this->process_id);
              if (this->process_id == 0)
                DLIO_PROFILER_LOGPRINT("Running DLIO Profiler with log_file %s data_dir %s and process %d",
                                       this->log_file.c_str(), this->data_dirs.c_str(), this->process_id);
            }
          }

        }
        bool finalize() {
          if (is_init && is_enabled) {
            DLIO_PROFILER_LOGINFO("Calling finalize", "");
            dlio_profiler::Singleton<DLIOLogger>::get_instance(false)->finalize();
            free_bindings();
            return true;
          }
          return false;
        }
    };
}  // namespace dlio_profiler


#endif //DLIO_PROFILER_DLIO_PROFILER_MAIN_H
