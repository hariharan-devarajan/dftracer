//
// Created by haridev on 10/8/23.
//

#include <dlio_profiler/dlio_profiler.h>
#include <dlio_profiler/core/dlio_profiler_main.h>

void initialize(const char *log_file, const char *data_dirs, int *process_id) {
  DLIO_PROFILER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_C_APP, log_file, data_dirs,
                                    process_id);
}

TimeResolution get_time() {
  return DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_C_APP)->get_time();
}

void log_event(const char *name, const char *cat, TimeResolution start_time, TimeResolution duration) {
  auto args = std::unordered_map<std::string, std::any>();
  DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_C_APP)->log(name, cat, start_time,
                                                                                                duration, args);
}

void finalize() {
  DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_C_APP)->finalize();
}