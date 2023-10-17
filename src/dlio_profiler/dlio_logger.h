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
#include <dlio_profiler/writer/base_writer.h>
#include <dlio_profiler/writer/chrome_writer.h>
#include <unistd.h>
#include <dlio_profiler/core/macro.h>
#include <dlio_profiler/utils/utils.h>
#include <sys/time.h>

typedef std::chrono::high_resolution_clock chrono;

class DLIOLogger {
private:
    bool throw_error, include_metadata;
    std::shared_ptr<dlio_profiler::BaseWriter> writer;
    bool is_init;
    ProcessID process_id;
public:
    DLIOLogger(bool init_log = false) : is_init(false), include_metadata(false) {
      char *dlio_profiler_meta = getenv(DLIO_PROFILER_INC_METADATA);
      if (dlio_profiler_meta != nullptr && strcmp(dlio_profiler_meta, "1") == 0) {
        include_metadata = true;
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

    inline void update_log_file(std::string log_file, ProcessID process_id = -1) {
      this->process_id = process_id;
      writer = std::make_shared<dlio_profiler::ChromeWriter>(-1);
      writer->initialize(log_file.data(), this->throw_error);
      this->is_init = true;
      DLIO_PROFILER_LOGINFO("Writing trace to %s", log_file.c_str());
    }

    inline TimeResolution get_time() {
      struct timeval tv{};
      gettimeofday(&tv,NULL);
      return 1000000 * tv.tv_sec + tv.tv_usec;
    }

    inline void log(std::string event_name, std::string category,
                    TimeResolution start_time, TimeResolution duration,
                    std::unordered_map<std::string, std::any> &metadata) {
      ThreadID tid = dlp_gettid() + this->process_id;
      writer->log(event_name, category, start_time, duration, metadata, this->process_id, tid);
    }

    inline bool has_metadata() {
      return this->include_metadata;
    }

    inline void finalize() {
      writer->finalize();
    }
};

#define DLIO_LOGGER_INIT() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()
#define DLIO_LOGGER_FINI() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()->finalize()
#define DLIO_LOGGER_UPDATE(value) if (trace && this->logger->has_metadata()) metadata.insert_or_assign(#value, value);
#define DLIO_LOGGER_START(entity)                               \
  auto pair = is_traced(entity, __FUNCTION__);                  \
  bool trace = pair.first;                                      \
  TimeResolution start_time = 0;                                \
  auto metadata = std::unordered_map<std::string, std::any>();  \
  if (trace) {                                                  \
    auto filename = pair.second;                                \
    DLIO_LOGGER_UPDATE(filename);                               \
    start_time = this->logger->get_time();                      \
  }
#define DLIO_LOGGER_END()                                 \
  if (trace) {                                                          \
    TimeResolution end_time = this->logger->get_time();                         \
    this->logger->log(__FUNCTION__, CATEGORY, start_time, end_time - start_time, metadata);  \
  }

#endif //DLIO_PROFILER_GENERIC_LOGGER_H
