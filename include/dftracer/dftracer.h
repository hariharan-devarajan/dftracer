//
// Created by haridev on 10/7/23.
//

#ifndef DFTRACER_DFTRACER_H
#define DFTRACER_DFTRACER_H

/**
 * Common to both C and CPP
 */
#include <dftracer/core/constants.h>
#include <dftracer/core/typedef.h>

#ifdef __cplusplus
extern "C" {
#endif
void initialize_main(const char *log_file, const char *data_dirs,
                     int *process_id);
void initialize_no_bind(const char *log_file, const char *data_dirs,
                        int *process_id);
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
#include <string>
#include <unordered_map>

// constants defined
__attribute__((unused)) static ConstEventType CPP_LOG_CATEGORY = "CPP_APP";

class DFTracer {
  bool initialized;
  ConstEventType name;
  ConstEventType cat;
  TimeResolution start_time;
  std::unordered_map<std::string, std::any> *metadata;

 public:
  DFTracer(ConstEventType _name, ConstEventType _cat);

  void update(const char *key, int value);

  void update(const char *key, const char *value);

  void finalize();

  ~DFTracer();
};

#define DFTRACER_CPP_INIT(log_file, data_dirs, process_id) \
  initialize_main(log_file, data_dirs, process_id);
#define DFTRACER_CPP_INIT_NO_BIND(log_file, data_dirs, process_id) \
  initialize_no_bind(log_file, data_dirs, process_id);
#define DFTRACER_CPP_FINI() finalize()
#define DFTRACER_CPP_FUNCTION() \
  DFTracer profiler_dft_fn = DFTracer((char *)__FUNCTION__, CPP_LOG_CATEGORY);

#define DFTRACER_CPP_REGION(name) \
  DFTracer profiler_##name = DFTracer(#name, CPP_LOG_CATEGORY);

#define DFTRACER_CPP_REGION_START(name) \
  DFTracer *profiler_##name = new DFTracer(#name, CPP_LOG_CATEGORY);

#define DFTRACER_CPP_REGION_END(name) delete profiler_##name

#define DFTRACER_CPP_FUNCTION_UPDATE(key, val) profiler_dft_fn.update(key, val);

#define DFTRACER_CPP_REGION_UPDATE(name, key, val) \
  profiler_##name.update(key, val);

#define DFTRACER_CPP_REGION_DYN_UPDATE(name, key, val) \
  profiler_##name->update(key, val);

extern "C" {
#endif
// C APIs

struct DFTracerData {
  void *profiler;
};

__attribute__((unused)) static ConstEventType C_LOG_CATEGORY = "C_APP";
struct DFTracerData *initialize_region(ConstEventType name, ConstEventType cat);
void finalize_region(struct DFTracerData *data);
void update_metadata_int(struct DFTracerData *data, const char *key, int value);
void update_metadata_string(struct DFTracerData *data, const char *key,
                            const char *value);

#define DFTRACER_C_INIT(log_file, data_dirs, process_id) \
  initialize_main(log_file, data_dirs, process_id);
#define DFTRACER_C_INIT_NO_BIND(log_file, data_dirs, process_id) \
  initialize_no_bind(log_file, data_dirs, process_id);
#define DFTRACER_C_FINI() finalize()

#define DFTRACER_C_FUNCTION_START() \
  struct DFTracerData *data_fn = initialize_region(__func__, C_LOG_CATEGORY);

#define DFTRACER_C_FUNCTION_END() finalize_region(data_fn);

#define DFTRACER_C_REGION_START(name) \
  struct DFTracerData *data_##name = initialize_region(#name, C_LOG_CATEGORY);

#define DFTRACER_C_REGION_END(name) finalize_region(data_##name);

#define DFTRACER_C_FUNCTION_UPDATE_INT(key, val) \
  update_metadata_int(data_fn, key, val);

#define DFTRACER_C_FUNCTION_UPDATE_STR(key, val) \
  update_metadata_string(data_fn, key, val);

#define DFTRACER_C_REGION_UPDATE_INT(name, key, val) \
  update_metadata_int(data_##name, key, val);

#define DFTRACER_C_REGION_UPDATE_STR(name, key, val) \
  update_metadata_string(data_##name, key, val);

#ifdef __cplusplus
}
#endif

#endif  // DFTRACER_DFTRACER_H
