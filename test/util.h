//
// Created by haridev on 10/21/23.
//

#ifndef DLIO_PROFILER_TEST_UTIL_H
#define DLIO_PROFILER_TEST_UTIL_H
#include <experimental/filesystem>
#include <cstring>

namespace fs = std::experimental::filesystem;
inline void init_log() {
  cpplogger::LoggerType logger_level;
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
  DLIO_PROFILER_LOGDEBUG("Enabling logging level %d", logger_level);
}
class Timer {
public:
    Timer() : elapsed_time(0) {}
    void resumeTime() { t1 = std::chrono::high_resolution_clock::now(); }
    double pauseTime() {
      auto t2 = std::chrono::high_resolution_clock::now();
      elapsed_time += std::chrono::duration<double>(t2 - t1).count();
      return elapsed_time;
    }
    double getElapsedTime() { return elapsed_time; }

private:
    std::chrono::high_resolution_clock::time_point t1;
    double elapsed_time;
};
#endif //DLIO_PROFILER_TEST_UTIL_H
