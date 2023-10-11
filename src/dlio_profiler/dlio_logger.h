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

typedef std::chrono::high_resolution_clock chrono;

class DLIOLogger {
private:
    TimeResolution library_start;
    bool throw_error, include_metadata;
    std::shared_ptr<dlio_profiler::BaseWriter> writer;
    bool is_init;
    int pid, tid;
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
      int fd = -1;
      std::string log_file;
      if (log_file.empty()) {
        char *dlio_profiler_log_dir = getenv("DLIO_PROFILER_LOG_DIR");
        if (dlio_profiler_log_dir == nullptr) {
          fd = fileno(stderr);
          log_file = "STDERR";
        } else {
          if (strcmp(dlio_profiler_log_dir, "STDERR") == 0) { // GCOV_EXCL_START
            fd = fileno(stderr);
            log_file = "STDERR";
          } else if (strcmp(dlio_profiler_log_dir, "STDOUT") == 0) {
            fd = fileno(stdout);
            log_file = "STDOUT";
          } else {
            int pid = dlp_getpid();
            log_file = std::string(dlio_profiler_log_dir) + "/" + "trace_ll_" + std::to_string(pid) + ".pfw";
          } // GCOV_EXCL_STOP
        }
      }
      update_log_file(log_file);
    }

    inline TimeResolution get_current_time() {
      return std::chrono::duration<TimeResolution>(
              std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    inline void update_log_file(std::string log_file, int process_id = -1, int tid = -1) {
      this->pid = process_id;
      this->tid = tid;
      writer = std::make_shared<dlio_profiler::ChromeWriter>(-1);
      writer->initialize(log_file.data(), this->throw_error);
      this->is_init = true;
      library_start = get_current_time();
      DLIO_PROFILER_LOGINFO("Writing trace to %s with time %f", log_file.c_str(), library_start);
    }

    inline TimeResolution get_time() {
      auto t = get_current_time() - library_start;
      return t;
    }

    inline void log(std::string event_name, std::string category,
                    TimeResolution start_time, TimeResolution duration,
                    std::unordered_map<std::string, std::any> &metadata) {
      writer->log(event_name, category, start_time, duration, metadata, pid, tid);
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
