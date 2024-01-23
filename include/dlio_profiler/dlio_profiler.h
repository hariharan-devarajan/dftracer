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
extern "C" {
#endif
void initialize_main(const char *log_file, const char *data_dirs, int *process_id);
void initialize_no_bind(const char *log_file, const char *data_dirs, int *process_id);
void finalize();
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * CPP Only
 */

// External Headers
#include <any>
#include <unordered_map>
#include <string>

// constants defined
static ConstEventType CPP_LOG_CATEGORY  = "CPP_APP";

class DLIOProfiler {
    bool initialized;
    ConstEventType name;
    ConstEventType cat;
    TimeResolution start_time;
    std::unordered_map<std::string, std::any>* metadata;
public:
    DLIOProfiler(ConstEventType _name, ConstEventType _cat);

    void update(const char *key, int value);

    void update(const char *key, const char *value);

    void finalize();

    ~DLIOProfiler();
};

#define DLIO_PROFILER_CPP_INIT(log_file, data_dirs, process_id)     \
  initialize_main(log_file, data_dirs, process_id);
#define DLIO_PROFILER_CPP_INIT_NO_BIND(log_file, data_dirs, process_id)     \
  initialize_no_bind(log_file, data_dirs, process_id);
#define DLIO_PROFILER_CPP_FINI()  \
  finalize()
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

static ConstEventType C_LOG_CATEGORY = "C_APP";
struct DLIOProfilerData* initialize_region(ConstEventType name, ConstEventType cat);
void finalize_region(struct DLIOProfilerData* data);
void update_metadata_int(struct DLIOProfilerData* data, const char *key, int value);
void update_metadata_string(struct DLIOProfilerData* data, const char *key, const char *value);

#define DLIO_PROFILER_C_INIT(log_file, data_dirs, process_id)     \
  initialize_main(log_file, data_dirs, process_id);
#define DLIO_PROFILER_C_INIT_NO_BIND(log_file, data_dirs, process_id)     \
  initialize_no_bind(log_file, data_dirs, process_id);
#define DLIO_PROFILER_C_FINI()                              \
  finalize()

#define DLIO_PROFILER_C_FUNCTION_START() \
  struct DLIOProfilerData* data_fn = initialize_region(__func__, C_LOG_CATEGORY);

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
