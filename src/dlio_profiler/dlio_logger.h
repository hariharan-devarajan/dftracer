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

typedef std::chrono::high_resolution_clock chrono;
class DLIOLogger {
private:
    std::unordered_map<char*, std::any> metadata;
    chrono::time_point library_start;
    chrono::time_point start_time;
    double elapsed_time;
    std::string event_name;
    std::string category;
    std::shared_ptr<dlio_profiler::BaseWriter> writer;
public:
    DLIOLogger():metadata(){
      library_start = std::chrono::high_resolution_clock::now();
      char* dlio_profiler_log_dir = getenv("DLIO_PROFILER_LOG_DIR");
      char* dlio_profiler_error = getenv("DLIO_PROFILER_ERROR");
      bool throw_error = false;
      if (dlio_profiler_error != nullptr && strcmp(dlio_profiler_log_dir, "1") == 0) {
        throw_error = true;
      }
      FILE* fp = NULL;
      std::string filename;
      if (dlio_profiler_log_dir == nullptr) {
        fp = stderr;
        filename = "STDERR";
      } else {
        if (strcmp(dlio_profiler_log_dir, "STDERR") == 0) {
          fp = stderr;
          filename = "STDERR";
        } else if (strcmp(dlio_profiler_log_dir, "STDOUT") == 0) {
          fp = stdout;
          filename = "STDOUT";
        } else {
          int pid = getpid();
          filename = std::string(dlio_profiler_log_dir) + "/" + "trace_ll_" + std::to_string(pid) + ".pfw" ;
          DLIO_PROFILER_LOGINFO("Writing trace to %s", filename.c_str());
        }
      }
      writer = std::make_shared<dlio_profiler::ChromeWriter>(fp);
      writer->initialize(filename.data(), throw_error);
    }
    inline void start(std::string _event_name, std::string _category) {
      start_time = std::chrono::high_resolution_clock::now();
      this->event_name = _event_name;
      this->category = _category;
    }
    template <typename T>
    inline void update_metadata(const char* key, const T value){
      metadata.insert_or_assign(key, value);
    }

    inline std::unordered_map<char*, std::any> get_metadata() {
      return metadata;
    }

    inline void stop(){
      auto end_time = std::chrono::high_resolution_clock::now();
      elapsed_time = std::chrono::duration<double>(end_time - start_time).count();
      auto ts = std::chrono::duration<double>(start_time - library_start).count();
      DLIO_PROFILER_LOGINFO("event logged {name:%s, cat:%s, ts:%f, dur:%f}",
                            this->event_name.c_str(), this->category.c_str(), ts, elapsed_time);
      writer->log(this->event_name, this->category, ts, elapsed_time, metadata);

    }
    inline void finalize() {
      writer->finalize();
    }
};
#define DLIO_LOGGER_INIT() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()
#define DLIO_LOGGER_FINI() \
  dlio_profiler::Singleton<DLIOLogger>::get_instance()->finalize();
#define DLIO_LOGGER_START(name, category, entity) \
  bool trace = is_traced(entity);             \
  if (trace) this->logger->start(name, category);
#define DLIO_LOGGER_UPDATE(key, value) \
  this->logger->update_metadata(key, value);
#define DLIO_LOGGER_END(entity) \
  if (trace) this->logger->stop();
#endif //DLIO_PROFILER_GENERIC_LOGGER_H
