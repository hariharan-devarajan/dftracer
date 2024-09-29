//
// Created by haridev on 10/8/23.
//

#include <dftracer/core/dftracer_main.h>
#include <dftracer/core/enumeration.h>
#include <dftracer/dftracer.h>

DFTracer::DFTracer(ConstEventNameType _name, ConstEventNameType _cat,
                   int event_type)
    : initialized(true),
      name(_name),
      cat(_cat),
      metadata(nullptr),
      event_type(event_type) {
  DFTRACER_LOG_DEBUG("DFTracer::DFTracer event %s cat %s ", _name, _cat);
  auto dftracer_core = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER,
                                               ProfileType::PROFILER_CPP_APP);
  if (dftracer_core != nullptr) {
    if (event_type == DF_DATA_EVENT) {
      if (dftracer_core->include_metadata)
        metadata = new std::unordered_map<std::string, std::any>();
      start_time = dftracer_core->get_time();
    }
  }
  dftracer_core->enter_event();
}
void DFTracer::update(const char *key, int value) {
  DFTRACER_LOG_DEBUG("DFTracer::update event %s cat %s  key %s value %d ", name,
                     cat, key, value);
  if (event_type == DF_DATA_EVENT) {
    auto dftracer_core = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER,
                                                 ProfileType::PROFILER_CPP_APP);
    if (dftracer_core != nullptr && dftracer_core->is_active() &&
        dftracer_core->include_metadata) {
      metadata->insert_or_assign(key, value);
    }
  }
}
void DFTracer::update(const char *key, const char *value) {
  DFTRACER_LOG_DEBUG("DFTracer::update event %s cat %s  key %s value %s ", name,
                     cat, key, value);
  if (event_type == DF_DATA_EVENT) {
    auto dftracer_core = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER,
                                                 ProfileType::PROFILER_CPP_APP);
    if (dftracer_core != nullptr && dftracer_core->is_active() &&
        dftracer_core->include_metadata) {
      metadata->insert_or_assign(key, value);
    }
  }
}
void DFTracer::finalize() {
  DFTRACER_LOG_DEBUG("DFTracer::finalize event %s cat %s", name, cat);
  auto dftracer_core = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER,
                                               ProfileType::PROFILER_CPP_APP);
  if (dftracer_core != nullptr && dftracer_core->is_active()) {
    if (event_type == DF_DATA_EVENT) {
      TimeResolution end_time = dftracer_core->get_time();
      dftracer_core->log(name, cat, start_time, end_time - start_time,
                         metadata);
    } else if (event_type == DF_METADATA_EVENT) {
      dftracer_core->log_metadata(name, cat);
    }

    dftracer_core->exit_event();
    if (dftracer_core->include_metadata) delete (metadata);
  }
  this->initialized = false;
}

DFTracer::~DFTracer() {
  DFTRACER_LOG_DEBUG("DFTracer::~DFTracer event %s cat %s", name, cat);
  if (this->initialized) finalize();
}

void initialize_main(const char *log_file, const char *data_dirs,
                     int *process_id) {
  DFTRACER_LOG_DEBUG("dftracer.initialize_main", "");
  DFTRACER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_INIT,
                               ProfileType::PROFILER_C_APP, log_file, data_dirs,
                               process_id);
}
void initialize_no_bind(const char *log_file, const char *data_dirs,
                        int *process_id) {
  DFTRACER_LOG_DEBUG("dftracer.initialize_no_bind", "");
  DFTRACER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_OTHER,
                               ProfileType::PROFILER_C_APP, log_file, data_dirs,
                               process_id);
}

struct DFTracerData *initialize_region(ConstEventNameType name,
                                       ConstEventNameType cat, int event_type) {
  DFTRACER_LOG_DEBUG("dftracer.initialize_region event %s cat %s", name, cat);
  auto data = new DFTracerData();
  data->profiler = new DFTracer(name, cat, event_type);
  return data;
}
void finalize_region(struct DFTracerData *data) {
  DFTRACER_LOG_DEBUG("dftracer.finalize_region", "");
  if (data != NULL) {
    auto profiler = (DFTracer *)data->profiler;
    if (profiler) {
      profiler->finalize();
      delete (profiler);
    }
    delete (data);
  }
}

void update_metadata_int(struct DFTracerData *data, const char *key,
                         int value) {
  DFTRACER_LOG_DEBUG("dftracer.update_metadata_int", "");
  if (data && data->profiler) {
    auto profiler = (DFTracer *)data->profiler;
    profiler->update(key, value);
  }
}
void update_metadata_string(struct DFTracerData *data, const char *key,
                            const char *value) {
  DFTRACER_LOG_DEBUG("dftracer.update_metadata_string", "");
  if (data && data->profiler) {
    auto profiler = (DFTracer *)data->profiler;
    profiler->update(key, value);
  }
}

TimeResolution get_time() {
  DFTRACER_LOG_DEBUG("dftracer.cpp.get_time", "");
  auto dftracer = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER,
                                          ProfileType::PROFILER_C_APP);
  if (dftracer != nullptr)
    return dftracer->get_time();
  else
    DFTRACER_LOG_ERROR("dftracer.cpp.get_time dftracer not initialized", "");
  return 0;
}

void log_event(ConstEventNameType name, ConstEventNameType cat,
               TimeResolution start_time, TimeResolution duration) {
  DFTRACER_LOG_DEBUG("dftracer.cpp.log_event", "");
  auto dftracer = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER,
                                          ProfileType::PROFILER_C_APP);
  if (dftracer != nullptr)
    dftracer->log(name, cat, start_time, duration, nullptr);
  else
    DFTRACER_LOG_ERROR("dftracer.cpp.log_event dftracer not initialized", "");
}

void finalize() {
  DFTRACER_LOG_DEBUG("dftracer.cpp.finalize", "");
  auto dftracer = DFTRACER_MAIN_SINGLETON(ProfilerStage::PROFILER_FINI,
                                          ProfileType::PROFILER_C_APP);
  if (dftracer != nullptr) {
    dftracer->finalize();
    dftracer::Singleton<dftracer::DFTracerCore>::finalize();
  }
}