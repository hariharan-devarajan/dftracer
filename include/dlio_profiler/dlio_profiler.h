//
// Created by haridev on 10/7/23.
//

#ifndef DLIO_PROFILER_DLIO_PROFILER_H
#define DLIO_PROFILER_DLIO_PROFILER_H

/**
 * Common to both C and CPP
 */
#include <dlio_profiler/core/typedef.h>
#include <dlio_profiler/core/constants.h>
#ifdef __cplusplus
/**
 * CPP Only
 */
// Internal Headers
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/core/dlio_profiler_main.h>

// External Headers
#include <any>
#include <unordered_map>
#include <string>

class DLIOProfiler {
    const char* name;
    TimeResolution start_time;
    std::unordered_map<std::string, std::any> metadata;
    std::shared_ptr<dlio_profiler::DLIOProfilerCore> dlio_profiler_core;
public:
    DLIOProfiler(const char* _name):name(_name), metadata(){
      dlio_profiler_core = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_CPP_APP);
      start_time = dlio_profiler_core->get_time();
    }
    inline void update(const char* key, int value) {
      if (dlio_profiler_core->is_active()) {
        metadata.insert_or_assign(key, value);
      }
    }
    inline void update(const char* key, const char* value) {
      if (dlio_profiler_core->is_active()) {
        metadata.insert_or_assign(key, value);
      }
    }
    ~DLIOProfiler() {
      if (dlio_profiler_core->is_active()) {
        TimeResolution end_time = dlio_profiler_core->get_time();
        dlio_profiler_core->log(name, CPP_LOG_CATEGORY, start_time, end_time - start_time, metadata);
      }
    }
};
#define DLIO_PROFILER_CPP_INIT(log_file, data_dirs, process_id)                                               \
DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_CPP_APP, log_file, data_dirs, process_id);
#define DLIO_PROFILER_CPP_FINI()                                               \
DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_CPP_APP);
#define DLIO_PROFILER_CPP_FUNCTION() \
DLIOProfiler profiler_dlio_fn = DLIOProfiler(__FUNCTION__);

#define DLIO_PROFILER_CPP_REGION(name)                           \
DLIOProfiler profiler_##name = DLIOProfiler(#name);

#define DLIO_PROFILER_CPP_REGION_START(name)                           \
DLIOProfiler* profiler_##name = new DLIOProfiler(#name);

#define DLIO_PROFILER_CPP_REGION_END(name)                           \
delete profiler_##name

extern "C" {
#endif
// C APIs
void initialize(const char * log_file, const char * data_dirs, int process_id);
TimeResolution get_time();
void log_event(const char * name, const char * cat, TimeResolution start_time, TimeResolution duration);
void finalize();

#define DLIO_PROFILER_C_INIT(log_file, data_dirs, process_id) \
  initialize(log_file, data_dirs, process_id);
#define DLIO_PROFILER_C_FINI()                              \
  finalize()

#define DLIO_PROFILER_C_FUNCTION_START() \
TimeResolution start_time_fn = get_time();

#define DLIO_PROFILER_C_FUNCTION_END() \
TimeResolution end_time_fn = get_time();  \
log_event(__FUNCTION__, C_LOG_CATEGORY, start_time_fn, end_time_fn - start_time_fn);

#define DLIO_PROFILER_C_REGION_START(name)        \
const char* name_##name = #name;                  \
TimeResolution start_time_##name = get_time();

#define DLIO_PROFILER_C_REGION_END(name)                              \
TimeResolution end_time_##name = get_time();                          \
TimeResolution duration_##name = end_time_##name - start_time_##name; \
log_event(name_##name, C_LOG_CATEGORY, start_time_##name, duration_##name);

#ifdef __cplusplus
}
#endif

#endif //DLIO_PROFILER_DLIO_PROFILER_H
