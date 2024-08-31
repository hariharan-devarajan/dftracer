//
// Created by haridev on 10/21/23.
//

#ifndef DFTRACER_TEST_UTIL_H
#define DFTRACER_TEST_UTIL_H
#include <dftracer/core/logging.h>

#include <cstring>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
inline void init_log() { DFTRACER_LOGGER_INIT(); }
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
