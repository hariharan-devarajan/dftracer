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
#include <dlio_profiler/core/dlio_profiler_main.h>
#include <dlio_profiler/core/enumeration.h>

// External Headers
#include <any>
#include <unordered_map>
#include <string>

// constants defined
ConstEventType CPP_LOG_CATEGORY="CPP_APP";

class DLIOProfiler {
    ConstEventType name;
    ConstEventType cat;
    TimeResolution start_time;
    std::unordered_map<std::string, std::any>* metadata;
    std::shared_ptr<dlio_profiler::DLIOProfilerCore> dlio_profiler_core;
public:
    DLIOProfiler(ConstEventType _name, ConstEventType _cat): name(_name), cat(_cat), metadata(nullptr) {
      dlio_profiler_core = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_CPP_APP);
      if (dlio_profiler_core != nullptr) {
        if (dlio_profiler_core->include_metadata) metadata = new std::unordered_map<std::string, std::any>();
        start_time = dlio_profiler_core->get_time();
      }
    }

    inline void update(const char *key, int value) {
      if (dlio_profiler_core != nullptr && dlio_profiler_core->is_active() && dlio_profiler_core->include_metadata) {
        metadata->insert_or_assign(key, value);
      }
    }

    inline void update(const char *key, const char *value) {
      if (dlio_profiler_core != nullptr && dlio_profiler_core->is_active() && dlio_profiler_core->include_metadata) {
        metadata->insert_or_assign(key, value);
      }
    }

    ~DLIOProfiler() {
      if (dlio_profiler_core != nullptr && dlio_profiler_core->is_active()) {
        TimeResolution end_time = dlio_profiler_core->get_time();
        dlio_profiler_core->log(name, cat, start_time, end_time - start_time, metadata);
        if (dlio_profiler_core->include_metadata) delete(metadata);
      }
    }
};

#define DLIO_PROFILER_CPP_INIT(log_file, data_dirs, process_id)                                               \
DLIO_PROFILER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_CPP_APP, log_file, data_dirs, process_id);
#define DLIO_PROFILER_CPP_FINI()                                               \
DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_CPP_APP)->finalize();
#define DLIO_PROFILER_CPP_FUNCTION() \
DLIOProfiler profiler_dlio_fn = DLIOProfiler((char*)__FUNCTION__, CPP_LOG_CATEGORY);

#define DLIO_PROFILER_CPP_REGION(name)                           \
DLIOProfiler profiler_##name = DLIOProfiler(#name, CPP_LOG_CATEGORY);

#define DLIO_PROFILER_CPP_REGION_START(name)                           \
DLIOProfiler* profiler_##name = new DLIOProfiler(#name, CPP_LOG_CATEGORY);

#define DLIO_PROFILER_CPP_REGION_END(name)                           \
delete profiler_##name

#define DLIO_PROFILER_CPP_FUNCTION_UPDATE(key, val) \
        profiler_dlio_fn.update(key, val);

#define DLIO_PROFILER_CPP_REGION_UPDATE(name, key, val) \
        profiler_##name.update(key, val);

#define DLIO_PROFILER_CPP_REGION_DYN_UPDATE(name, key, val) \
        profiler_##name->update(key, val);


extern "C" {
#endif
// C APIs

struct DLIOProfilerData {
  void* profiler;
};

ConstEventType C_LOG_CATEGORY="C_APP";

void initialize(const char *log_file, const char *data_dirs, int *process_id);
struct DLIOProfilerData* initialize_region(ConstEventType name, ConstEventType cat);
void finalize_region(struct DLIOProfilerData* data);
void update_metadata_int(struct DLIOProfilerData* data, const char *key, int value);
void update_metadata_string(struct DLIOProfilerData* data, const char *key, const char *value);
void finalize();

#define DLIO_PROFILER_C_INIT(log_file, data_dirs, process_id) \
  initialize(log_file, data_dirs, process_id);
#define DLIO_PROFILER_C_FINI()                              \
  finalize()

#define DLIO_PROFILER_C_FUNCTION_START() \
  struct DLIOProfilerData* data_fn = initialize_region(__FUNCTION__, C_LOG_CATEGORY);

#define DLIO_PROFILER_C_FUNCTION_END() \
  finalize_region(data_fn);

#define DLIO_PROFILER_C_REGION_START(name) \
  struct DLIOProfilerData* data_##name = initialize_region(#name, C_LOG_CATEGORY);

#define DLIO_PROFILER_C_REGION_END(name)                              \
  finalize_region(data_##name);

#define DLIO_PROFILER_C_FUNCTION_UPDATE_INT(key, val) \
        update_metadata_int(data_fn, key, val);

#define DLIO_PROFILER_C_FUNCTION_UPDATE_STR(key, val) \
        update_metadata_string(data_fn, key, val);

#define DLIO_PROFILER_C_REGION_UPDATE_INT(name, key, val) \
        update_metadata_int(data_##name, key, val);

#define DLIO_PROFILER_C_REGION_UPDATE_STR(name, key, val) \
        update_metadata_string(data_##name, key, val);

#ifdef __cplusplus
}
#endif

#endif //DLIO_PROFILER_DLIO_PROFILER_H
