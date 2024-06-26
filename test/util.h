//
// Created by haridev on 10/21/23.
//

#ifndef DFTRACER_TEST_UTIL_H
#define DFTRACER_TEST_UTIL_H
#include <cstring>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
inline void init_log() {
  cpplogger::LoggerType logger_level;
  char *dftracer_log_level = getenv(DFTRACER_LOG_LEVEL);
  if (dftracer_log_level == nullptr) {  // GCOV_EXCL_START
    logger_level = cpplogger::LoggerType::LOG_ERROR;
  } else {
    if (strcmp(dftracer_log_level, "ERROR") == 0) {
      logger_level = cpplogger::LoggerType::LOG_ERROR;
    } else if (strcmp(dftracer_log_level, "INFO") == 0) {
      logger_level = cpplogger::LoggerType::LOG_INFO;
    } else if (strcmp(dftracer_log_level, "DEBUG") == 0) {
      logger_level = cpplogger::LoggerType::LOG_WARN;
    }
  }  // GCOV_EXCL_STOP
  DFTRACER_LOGGER->level = logger_level;
  DFTRACER_LOGDEBUG("Enabling logging level %d", logger_level);
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
#endif  // DFTRACER_TEST_UTIL_H
