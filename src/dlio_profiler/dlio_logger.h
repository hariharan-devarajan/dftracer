//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_GENERIC_LOGGER_H
#define DLIO_PROFILER_GENERIC_LOGGER_H

#include <dlio_profiler/core/macro.h>
#include <dlio_profiler/core/singleton.h>
#include <dlio_profiler/utils/configuration_manager.h>
#include <dlio_profiler/utils/utils.h>
#include <dlio_profiler/writer/chrome_writer.h>
#include <sys/time.h>
#include <unistd.h>

#include <any>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <unordered_map>

typedef std::chrono::high_resolution_clock chrono;

class DLIOLogger {
 private:
  bool throw_error;
  bool is_init, dlio_profiler_tid;
  ProcessID process_id;
  std::shared_ptr<dlio_profiler::ChromeWriter> writer;
  uint32_t level;
  std::vector<int> index_stack;
  std::atomic_int index;

 public:
  bool include_metadata;
  DLIOLogger(bool init_log = false)
      : is_init(false),
        dlio_profiler_tid(false),
        level(0),
        index_stack(),
        index(0),
        include_metadata(false) {
    DLIO_PROFILER_LOGDEBUG("DLIOLogger.DLIOLogger", "");
    auto conf = dlio_profiler::Singleton<
        dlio_profiler::ConfigurationManager>::get_instance();
    include_metadata = conf->metadata;
    dlio_profiler_tid = conf->tids;
    throw_error = conf->throw_error;
    this->is_init = true;
  }
  ~DLIOLogger() { DLIO_PROFILER_LOGDEBUG("Destructing DLIOLogger", ""); }
  inline void update_log_file(std::string log_file, ProcessID process_id = -1) {
    DLIO_PROFILER_LOGDEBUG("DLIOLogger.update_log_file %s", log_file.c_str());
    this->process_id = process_id;
    this->writer =
        dlio_profiler::Singleton<dlio_profiler::ChromeWriter>::get_instance();
    if (this->writer != nullptr) {
      this->writer->initialize(log_file.data(), this->throw_error);
    }
    this->is_init = true;
    DLIO_PROFILER_LOGINFO("Writing trace to %s", log_file.c_str());
  }

  inline void enter_event() {
    index++;
    level++;
    index_stack.push_back(index.load());
  }

  inline void exit_event() {
    level--;
    index_stack.pop_back();
  }

  inline TimeResolution get_time() {
    DLIO_PROFILER_LOGDEBUG("DLIOLogger.get_time", "");
    struct timeval tv {};
    gettimeofday(&tv, NULL);
    TimeResolution t = 1000000 * tv.tv_sec + tv.tv_usec;
    return t;
  }

  inline void log(ConstEventType event_name, ConstEventType category,
                  TimeResolution start_time, TimeResolution duration,
                  std::unordered_map<std::string, std::any> *metadata) {
    DLIO_PROFILER_LOGDEBUG("DLIOLogger.log", "");
    ThreadID tid = 0;
    if (dlio_profiler_tid) {
      tid = dlp_gettid() + this->process_id;
    }
    if (metadata != nullptr) {
      metadata->insert_or_assign("level", level);
      int parent_index_value = -1;
      if (level > 1) {
        parent_index_value = index_stack[level - 2];
      }
      metadata->insert_or_assign("p_idx", parent_index_value);
    }
    if (this->writer != nullptr) {
      this->writer->log(index_stack[level - 1], event_name, category,
                        start_time, duration, metadata, this->process_id, tid);
    } else {
      DLIO_PROFILER_LOGERROR("DLIOLogger.log writer not initialized", "");
    }
  }

  inline void finalize() {
    DLIO_PROFILER_LOGDEBUG("DLIOLogger.finalize", "");
    if (this->writer != nullptr) {
      writer->finalize(index);
      DLIO_PROFILER_LOGINFO("Released Logger", "");
    } else {
      DLIO_PROFILER_LOGWARN("DLIOLogger.finalize writer not initialized", "");
    }
  }
};

#define DLIO_LOGGER_INIT() dlio_profiler::Singleton<DLIOLogger>::get_instance()
#define DLIO_LOGGER_FINI() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()->finalize()
#define DLIO_LOGGER_UPDATE(value)              \
  if (trace && this->logger->include_metadata) \
    metadata->insert_or_assign(#value, value);
#define DLIO_LOGGER_START(entity)                                 \
  DLIO_PROFILER_LOGDEBUG("Calling function %s", __FUNCTION__);    \
  const char *fname = is_traced(entity, __FUNCTION__);            \
  bool trace = fname != nullptr;                                  \
  TimeResolution start_time = 0;                                  \
  std::unordered_map<std::string, std::any> *metadata = nullptr;  \
  if (trace) {                                                    \
    if (this->logger->include_metadata) {                         \
      metadata = new std::unordered_map<std::string, std::any>(); \
      DLIO_LOGGER_UPDATE(fname);                                  \
    }                                                             \
    this->logger->enter_event();                                  \
    start_time = this->logger->get_time();                        \
  }
#define DLIO_LOGGER_END()                                         \
  if (trace) {                                                    \
    TimeResolution end_time = this->logger->get_time();           \
    this->logger->log((char *)__FUNCTION__, CATEGORY, start_time, \
                      end_time - start_time, metadata);           \
    this->logger->exit_event();                                   \
    if (this->logger->include_metadata) delete (metadata);        \
  }

#endif  // DLIO_PROFILER_GENERIC_LOGGER_H
