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
#include <dlio_profiler/macro.h>
#include <dlio_profiler/core/common.h>

typedef std::chrono::high_resolution_clock chrono;
class DLIOLogger {
private:
    TimeResolution library_start;
    bool throw_error;
    std::shared_ptr<dlio_profiler::BaseWriter> writer;
    bool is_init;
public:
    DLIOLogger(bool init_log = false):is_init(false) {
      char *dlio_profiler_error = getenv("DLIO_PROFILER_ERROR");
      if (dlio_profiler_error != nullptr && strcmp(dlio_profiler_error, "1") == 0) {
        throw_error = true;
      }
      if (init_log) {
        this->is_init=true;
        FILE *fp = NULL;
        std::string log_file;
        if (log_file.empty()) {
          char *dlio_profiler_log_dir = getenv("DLIO_PROFILER_LOG_DIR");
          if (dlio_profiler_log_dir == nullptr) {
            fp = stderr;
            log_file = "STDERR";
          } else {
            if (strcmp(dlio_profiler_log_dir, "STDERR") == 0) {
              fp = stderr;
              log_file = "STDERR";
            } else if (strcmp(dlio_profiler_log_dir, "STDOUT") == 0) {
              fp = stdout;
              log_file = "STDOUT";
            } else {
              int pid = getpid();
              log_file = std::string(dlio_profiler_log_dir) + "/" + "trace_ll_" + std::to_string(pid) + ".pfw";
            }
          }
        }
        update_log_file(log_file);
      }
    }
    inline TimeResolution get_current_time(){
      return std::chrono::duration<TimeResolution>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    inline void update_log_file(std::string log_file) {
      writer = std::make_shared<dlio_profiler::ChromeWriter>(nullptr);
      writer->initialize(log_file.data(), this->throw_error);
      this->is_init=true;
      library_start = get_current_time();
      DLIO_PROFILER_LOGPRINT("Writing trace to %s with time %f", log_file.c_str(), library_start);
    }
    inline TimeResolution get_time() {
      auto t =  get_current_time() - library_start;
      DLIO_PROFILER_LOGINFO("Getting time %f", t);
      return t;
    }
    inline void log(std::string event_name, std::string category,
                    TimeResolution start_time, TimeResolution duration,
                    std::unordered_map<std::string, std::any> &metadata) {
      writer->log(event_name, category, start_time, duration, metadata);
    }
    inline void finalize() {
      if (this->is_init) {
        writer->finalize();
      }
    }
};
#define DLIO_LOGGER_INIT() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()
#define DLIO_LOGGER_FINI() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()->finalize()
#define DLIO_LOGGER_START(entity)                               \
  bool trace = is_traced(entity);                               \
  TimeResolution start_time = 0;                                        \
  auto metadata = std::unordered_map<std::string, std::any>();  \
  if (trace) start_time = this->logger->get_time();
#define DLIO_LOGGER_UPDATE(value) if (trace) metadata.insert_or_assign(#value, value);
#define DLIO_LOGGER_END()                                 \
  if (trace) {                                                          \
    TimeResolution end_time = this->logger->get_time();                         \
    this->logger->log(__FUNCTION__, CATEGORY, start_time, end_time - start_time, metadata);  \
  }

#endif //DLIO_PROFILER_GENERIC_LOGGER_H
