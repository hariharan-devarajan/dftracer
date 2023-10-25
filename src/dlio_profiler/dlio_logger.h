//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_GENERIC_LOGGER_H
#define DLIO_PROFILER_GENERIC_LOGGER_H

#include <chrono>
#include <unordered_map>
#include <any>
#include <cstring>
#include <dlio_profiler/core/singleton.h>
#include <dlio_profiler/writer/chrome_writer.h>
#include <unistd.h>
#include <dlio_profiler/core/macro.h>
#include <dlio_profiler/utils/utils.h>
#include <sys/time.h>
typedef std::chrono::high_resolution_clock chrono;


class DLIOLogger {
private:
    bool throw_error;
    bool is_init,dlio_profiler_tid;
    ProcessID process_id;
public:
    bool include_metadata;
    DLIOLogger(bool init_log = false) : is_init(false), include_metadata(false), dlio_profiler_tid(false) {
      DLIO_PROFILER_LOGDEBUG("DLIOLogger.DLIOLogger","");
      char *dlio_profiler_meta = getenv(DLIO_PROFILER_INC_METADATA);
      if (dlio_profiler_meta != nullptr && strcmp(dlio_profiler_meta, "1") == 0) {
        include_metadata = true;
      }
      char *dlio_profiler_tid_str = getenv(DLIO_PROFILER_DISABLE_TIDS);
      if (dlio_profiler_tid_str == nullptr || strcmp(dlio_profiler_tid_str, "0") == 0) {
        dlio_profiler_tid = true;
      }
      char *dlio_profiler_error = getenv("DLIO_PROFILER_ERROR");
      if (dlio_profiler_error != nullptr && strcmp(dlio_profiler_error, "1") == 0) {
        throw_error = true; // GCOVR_EXCL_LINE
      }
      this->is_init = true;
      std::string log_file;
      if (!log_file.empty()) {
        update_log_file(log_file);
      }
    }
    ~DLIOLogger(){DLIO_PROFILER_LOGDEBUG("Destructing DLIOLogger","");}
    inline void update_log_file(std::string log_file, ProcessID process_id = -1) {
      DLIO_PROFILER_LOGDEBUG("DLIOLogger.update_log_file","");
      this->process_id = process_id;
      auto writer = dlio_profiler::Singleton<dlio_profiler::ChromeWriter>::get_instance(-1);
      if (writer != nullptr) {
        writer->initialize(log_file.data(), this->throw_error);
      }
      this->is_init = true;
      DLIO_PROFILER_LOGINFO("Writing trace to %s", log_file.c_str());
    }

    inline TimeResolution get_time() {
      DLIO_PROFILER_LOGDEBUG("DLIOLogger.get_time","");
      struct timeval tv{};
      gettimeofday(&tv,NULL);
      TimeResolution t = 1000000 * tv.tv_sec + tv.tv_usec;
      return t;
    }

    void log(ConstEventType event_name, ConstEventType category,
                    TimeResolution start_time, TimeResolution duration,
                    std::unordered_map<std::string, std::any> *metadata);

    void finalize();
};

#define DLIO_LOGGER_INIT() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()
#define DLIO_LOGGER_FINI() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()->finalize()
#define DLIO_LOGGER_UPDATE(value) if (trace && this->logger->include_metadata) metadata->insert_or_assign(#value, value);
#define DLIO_LOGGER_START(entity) \
  DLIO_PROFILER_LOGDEBUG("Calling function %s",__FUNCTION__); \
  const char* fname = is_traced(entity, __FUNCTION__); \
  bool trace = fname != nullptr;                       \
  TimeResolution start_time = 0;                                \
  std::unordered_map<std::string, std::any> *metadata = nullptr;          \
  if (trace) {                                                  \
    if (this->logger->include_metadata) {                         \
      metadata = new std::unordered_map<std::string, std::any>();                                           \
      DLIO_LOGGER_UPDATE(fname);                                               \
    }                                                            \
    start_time = this->logger->get_time();                      \
  }
#define DLIO_LOGGER_END()                                 \
  if (trace) {                                                          \
    TimeResolution end_time = this->logger->get_time();   \
    this->logger->log((char*)__FUNCTION__, CATEGORY, start_time, end_time - start_time, metadata); \
    if (this->logger->include_metadata) delete(metadata);                    \
  }

#endif //DLIO_PROFILER_GENERIC_LOGGER_H
