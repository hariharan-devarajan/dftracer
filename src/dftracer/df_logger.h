//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_GENERIC_LOGGER_H
#define DFTRACER_GENERIC_LOGGER_H

#include <dftracer/core/macro.h>
#include <dftracer/core/singleton.h>
#include <dftracer/utils/configuration_manager.h>
#include <dftracer/utils/utils.h>
#include <dftracer/writer/chrome_writer.h>
#include <sys/time.h>
#include <unistd.h>

#include <any>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <unordered_map>

typedef std::chrono::high_resolution_clock chrono;

class DFTLogger {
 private:
  bool throw_error;
  bool is_init, dftracer_tid;
  ProcessID process_id;
  std::shared_ptr<dftracer::ChromeWriter> writer;
  uint32_t level;
  std::vector<int> index_stack;
  std::atomic_int index;
  bool has_entry;

 public:
  bool include_metadata;
  DFTLogger(bool init_log = false)
      : is_init(false),
        dftracer_tid(false),
        level(0),
        index_stack(),
        index(0),
        has_entry(false),
        include_metadata(false) {
    DFTRACER_LOGDEBUG("DFTLogger.DFTLogger", "");
    auto conf =
        dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
    include_metadata = conf->metadata;
    dftracer_tid = conf->tids;
    throw_error = conf->throw_error;
    this->is_init = true;
  }
  ~DFTLogger() {
    index_stack.clear();
    DFTRACER_LOGDEBUG("Destructing DFTLogger", "");
  }
  inline void update_log_file(std::string log_file, ProcessID process_id = -1) {
    DFTRACER_LOGDEBUG("DFTLogger.update_log_file %s", log_file.c_str());
    this->process_id = process_id;
    this->writer = dftracer::Singleton<dftracer::ChromeWriter>::get_instance();
    if (this->writer != nullptr) {
      this->writer->initialize(log_file.data(), this->throw_error);
    }
    this->is_init = true;
    DFTRACER_LOGINFO("Writing trace to %s", log_file.c_str());
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
    DFTRACER_LOGDEBUG("DFTLogger.get_time", "");
    struct timeval tv {};
    gettimeofday(&tv, NULL);
    TimeResolution t = 1000000 * tv.tv_sec + tv.tv_usec;
    return t;
  }

  inline void log(ConstEventType event_name, ConstEventType category,
                  TimeResolution start_time, TimeResolution duration,
                  std::unordered_map<std::string, std::any> *metadata) {
    DFTRACER_LOGDEBUG("DFTLogger.log", "");
    ThreadID tid = 0;
    if (dftracer_tid) {
      tid = df_gettid() + this->process_id;
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
      has_entry = true;
    } else {
      DFTRACER_LOGERROR("DFTLogger.log writer not initialized", "");
    }
  }

  inline void finalize() {
    DFTRACER_LOGDEBUG("DFTLogger.finalize", "");
    if (this->writer != nullptr) {
      writer->finalize(has_entry);
      DFTRACER_LOGINFO("Released Logger", "");
    } else {
      DFTRACER_LOGWARN("DFTLogger.finalize writer not initialized", "");
    }
  }
};

#define DFT_LOGGER_INIT() dftracer::Singleton<DFTLogger>::get_instance()
#define DFT_LOGGER_FINI() \
  dftracer::Singleton<DFTLogger>::get_instance()->finalize()
#define DFT_LOGGER_UPDATE(value)               \
  if (trace && this->logger->include_metadata) \
    metadata->insert_or_assign(#value, value);
#define DFT_LOGGER_START(entity)                                  \
  DFTRACER_LOGDEBUG("Calling function %s", __FUNCTION__);         \
  const char *fname = is_traced(entity, __FUNCTION__);            \
  bool trace = fname != nullptr;                                  \
  TimeResolution start_time = 0;                                  \
  std::unordered_map<std::string, std::any> *metadata = nullptr;  \
  if (trace) {                                                    \
    if (this->logger->include_metadata) {                         \
      metadata = new std::unordered_map<std::string, std::any>(); \
      DFT_LOGGER_UPDATE(fname);                                   \
    }                                                             \
    this->logger->enter_event();                                  \
    start_time = this->logger->get_time();                        \
  }
#define DFT_LOGGER_END()                                          \
  if (trace) {                                                    \
    TimeResolution end_time = this->logger->get_time();           \
    this->logger->log((char *)__FUNCTION__, CATEGORY, start_time, \
                      end_time - start_time, metadata);           \
    this->logger->exit_event();                                   \
    if (this->logger->include_metadata) delete (metadata);        \
  }

#endif  // DFTRACER_GENERIC_LOGGER_H
