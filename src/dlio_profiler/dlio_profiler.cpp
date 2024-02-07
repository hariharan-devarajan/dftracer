//
// Created by haridev on 10/8/23.
//

#include <dlio_profiler/dlio_profiler.h>
#include <dlio_profiler/core/dlio_profiler_main.h>
#include <dlio_profiler/core/enumeration.h>

DLIOProfiler::DLIOProfiler(ConstEventType _name, ConstEventType _cat): name(_name), cat(_cat), metadata(nullptr), initialized(true) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfiler::DLIOProfiler event %s cat %s ", _name, _cat);
  auto dlio_profiler_core = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_CPP_APP);
  if (dlio_profiler_core != nullptr) {
    if (dlio_profiler_core->include_metadata) metadata = new std::unordered_map<std::string, std::any>();
    start_time = dlio_profiler_core->get_time();
  }
}
void DLIOProfiler::update(const char *key, int value) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfiler::update event %s cat %s  key %s value %d ", name, cat, key, value);
  auto dlio_profiler_core = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_CPP_APP);
  if (dlio_profiler_core != nullptr && dlio_profiler_core->is_active() && dlio_profiler_core->include_metadata) {
    metadata->insert_or_assign(key, value);
  }
}
void DLIOProfiler::update(const char *key, const char *value) {
  DLIO_PROFILER_LOGDEBUG("DLIOProfiler::update event %s cat %s  key %s value %s ", name, cat, key, value);
  auto dlio_profiler_core = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_CPP_APP);
  if (dlio_profiler_core != nullptr && dlio_profiler_core->is_active() && dlio_profiler_core->include_metadata) {
    metadata->insert_or_assign(key, value);
  }
}
void DLIOProfiler::finalize() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfiler::finalize event %s cat %s", name, cat);
  auto dlio_profiler_core = DLIO_PROFILER_MAIN_SINGLETON(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_CPP_APP);
  if (dlio_profiler_core != nullptr && dlio_profiler_core->is_active()) {
    TimeResolution end_time = dlio_profiler_core->get_time();
    dlio_profiler_core->log(name, cat, start_time, end_time - start_time, metadata);
    if (dlio_profiler_core->include_metadata) delete(metadata);
  }
  this->initialized = false;
}

DLIOProfiler::~DLIOProfiler() {
  DLIO_PROFILER_LOGDEBUG("DLIOProfiler::~DLIOProfiler event %s cat %s", name, cat);
  if (this->initialized) finalize();
}

void initialize_main(const char *log_file, const char *data_dirs, int *process_id) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.initialize_main","");
  DLIO_PROFILER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_C_APP, log_file, data_dirs,
                                    process_id);
}
void initialize_no_bind(const char *log_file, const char *data_dirs, int *process_id) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.initialize_no_bind","");
  DLIO_PROFILER_MAIN_SINGLETON_INIT(ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_C_APP, log_file, data_dirs,
                                    process_id);
}

struct DLIOProfilerData* initialize_region(ConstEventType name, ConstEventType cat) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.initialize_region event %s cat %s",name, cat);
  auto data = new DLIOProfilerData();
  data->profiler = new DLIOProfiler(name, C_LOG_CATEGORY);
  return data;
}
void finalize_region(struct DLIOProfilerData* data) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.finalize_region","");
  if (data != NULL) {
    auto profiler = (DLIOProfiler*)data->profiler;
    if (profiler) {
      profiler->finalize();
      delete(profiler);
    }
    delete(data);
  }
}

void update_metadata_int(struct DLIOProfilerData* data, const char *key, int value) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.update_metadata_int","");
  if (data && data->profiler) {
    auto profiler = (DLIOProfiler*) data->profiler;
    profiler->update(key, value);
  }
}
void update_metadata_string(struct DLIOProfilerData* data, const char *key, const char *value) {
  DLIO_PROFILER_LOGDEBUG("dlio_profiler.update_metadata_string","");
  if (data && data->profiler) {
    auto profiler = (DLIOProfiler*) data->profiler;
    profiler->update(key, value);
  }
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