//
// Created by haridev on 10/8/23.
//

#include <dlio_profiler/dlio_profiler.h>
#include <dlio_profiler/core/dlio_profiler_main.h>

void initialize(const char *log_file, const char *data_dirs, int *process_id) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.cpp.initialize","");
  DLIO_PROFILER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_C_APP, log_file, data_dirs,
                                    process_id);
}

TimeResolution get_time() {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.cpp.get_time","");
  auto dlio_profiler = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_C_APP);
  if (dlio_profiler != nullptr) return dlio_profiler->get_time();
  else DLIO_PROFILER_LOGERROR("dlio_profiler.cpp.get_time dlio_profiler not initialized","");
  return 0;
}

void log_event(ConstEventType name,ConstEventType cat, TimeResolution start_time, TimeResolution duration) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.cpp.log_event","");
  auto dlio_profiler = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_C_APP);
  if (dlio_profiler != nullptr) dlio_profiler->log(name, cat, start_time, duration, nullptr);
  else DLIO_PROFILER_LOGERROR("dlio_profiler.cpp.log_event dlio_profiler not initialized","");
}

void finalize() {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.cpp.finalize","");
  auto dlio_profiler = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_C_APP);
  if (dlio_profiler != nullptr) {
    dlio_profiler->finalize();
    dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::finalize();
  }
}