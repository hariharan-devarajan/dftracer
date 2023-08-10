//
// Created by haridev on 3/28/23.
//

#include <dlio_profiler/dlio_profiler.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include<algorithm>
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/core/common.h>
namespace dlio_profiler {
    bool init = false;
}

bool is_init() {return dlio_profiler::init;}
void set_init(bool _init) { dlio_profiler::init = _init;}
void dlio_profiler_init(void) {
  bool init_log = false;
  char *ld_preload = getenv("LD_PRELOAD");
  if (ld_preload != nullptr && std::string(ld_preload).find("libdlio_profiler.so") != std::string::npos) {
    init_log = true;
  }
  if (!is_init()) {
    char *dlio_profiler_enable = getenv("DLIO_PROFILER_ENABLE");
    if (dlio_profiler_enable == nullptr || strcmp(dlio_profiler_enable, "1") == 0) {
      char *dlio_profiler_debug = getenv("DLIO_PROFILER_DEBUG");
      if (dlio_profiler_debug != nullptr) {
        if (strcmp(dlio_profiler_debug, "1") == 0) {
          std::string sp;
          std::ifstream("/proc/self/cmdline") >> sp;
          std::replace(sp.begin(), sp.end() - 1, '\000', ' ');
          fprintf(stderr, "Connect to pid %d %s\n", getpid(), sp.c_str());
          fflush(stderr);
          getchar();
        }
      }
      DLIO_PROFILER_LOGINFO("constructor", "");
      char *dlio_profiler_log_level = getenv("DLIO_PROFILER_LOG_LEVEL");
      if (dlio_profiler_log_level == nullptr) {
        DLIO_PROFILER_LOGGER->level = cpplogger::LoggerType::LOG_ERROR;
        DLIO_PROFILER_LOGINFO("Enabling ERROR loggin", "");
      } else {
        if (strcmp(dlio_profiler_log_level, "ERROR") == 0) {
          DLIO_PROFILER_LOGGER->level = cpplogger::LoggerType::LOG_ERROR;
          DLIO_PROFILER_LOGINFO("Enabling ERROR loggin", "");
        } else if (strcmp(dlio_profiler_log_level, "INFO") == 0) {
          DLIO_PROFILER_LOGGER->level = cpplogger::LoggerType::LOG_INFO;
          DLIO_PROFILER_LOGINFO("Enabling INFO loggin", "");
        } else if (strcmp(dlio_profiler_log_level, "DEBUG") == 0) {
          DLIO_PROFILER_LOGINFO("Enabling DEBUG loggin", "");
          DLIO_PROFILER_LOGGER->level = cpplogger::LoggerType::LOG_WARN;
        }
      }
      char *dlio_profiler_priority_str = getenv("DLIO_PROFILER_GOTCHA_PRIORITY");
      int dlio_profiler_priority = 1;
      if (dlio_profiler_priority_str != nullptr) {
        dlio_profiler_priority = atoi(dlio_profiler_priority_str);
      }
      /*
      dlio_profiler::Singleton<DLIOLogger>::get_instance(init_log);
      brahma_gotcha_wrap("dlio_profiler", dlio_profiler_priority);
      auto posix_instance = brahma::POSIXDLIOProfiler::get_instance();
      auto stdio_instance = brahma::STDIODLIOProfiler::get_instance();
      char *dlio_profiler_dir = getenv("DLIO_PROFILER_DIR");
      if (dlio_profiler_dir != nullptr) {
        auto paths = split(dlio_profiler_dir, ':');
        for (const auto &path:paths) {
          DLIO_PROFILER_LOGINFO("Profiler will trace %s\n", path.c_str());
          posix_instance->trace(path);
          stdio_instance->trace(path);
        }
      }
       */
    }
    set_init(true);
  }
  size_t thread_hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
  DLIO_PROFILER_LOGINFO("Running DLIO Profiler on thread %ld", thread_hash);

}
void dlio_profiler_fini(void) {
  dlio_profiler::Singleton<DLIOLogger>::get_instance(false)->finalize();
  if (is_init()) {
    free_bindings();
    set_init(false);
  }
}