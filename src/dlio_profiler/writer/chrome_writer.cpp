//
// Created by haridev on 3/28/23.
//

#include <dlio_profiler/writer/chrome_writer.h>
#include <fcntl.h>
#include <dlio_profiler/macro.h>
#include <cassert>
#include <unistd.h>
#include <thread>
#include <sstream>
#include <cmath>

#define ERROR(cond, format, ...) \
  DLIO_PROFILER_LOGERROR(format, __VA_ARGS__); \
  if (this->throw_error) assert(cond);
void dlio_profiler::ChromeWriter::initialize(char *filename, bool throw_error) {
  this->throw_error = throw_error;
  this->filename = filename;
}

void
dlio_profiler::ChromeWriter::log(std::string &event_name, std::string &category, TimeResolution &start_time, TimeResolution &duration,
                                 std::unordered_map<std::string, std::any> &metadata, int process_id) {
  if (is_first_write) {
    if (this->fp == NULL) {
      file_mtx.lock();
      fp = fopen(filename.c_str(), "w+");
      file_mtx.unlock();
    }
    if (fp == nullptr) {
      ERROR(fp == nullptr,"unable to create log file %s", filename.c_str());
    } else {
      std::string data = "[\n";
      file_mtx.lock();
      auto written_elements = fwrite(data.c_str(), data.size(), sizeof(char), fp);
      file_mtx.unlock();
      if (written_elements != 1) {
        ERROR(written_elements != 1, "unable to initialize log file %s", filename.c_str());
      }
    }
    is_first_write = false;
  }
  if (fp != nullptr) {
    std::string json = convert_json(event_name, category, start_time, duration, metadata, process_id);
    file_mtx.lock();
    auto written_elements = fwrite(json.c_str(), json.size(), sizeof(char), fp);
    fflush(fp);
    file_mtx.unlock();
    if (written_elements != 1) {
      ERROR(written_elements != 1, "unable to write to log file %s", filename.c_str());
    }
  }
}

void dlio_profiler::ChromeWriter::finalize() {
  if (fp != nullptr) {
    int status = fclose(fp);
    if (status != 0) {
      ERROR(status != -1, "unable to close log file %d", filename.c_str());
    }
  }
}


std::string
dlio_profiler::ChromeWriter::convert_json(std::string &event_name, std::string &category, TimeResolution start_time,
                                          TimeResolution duration, std::unordered_map<std::string, std::any> &metadata,
                                          int process_id) {
  std::stringstream all_stream;
  int tid, pid;
  if (process_id == -1) {
    tid = std::hash<std::thread::id>{}(std::this_thread::get_id()) % 100000;
    pid = getpid();
  } else {
    pid = process_id;
    tid = getpid() + std::hash<std::thread::id>{}(std::this_thread::get_id()) % 100000;
  }
  auto start_sec = std::chrono::duration<TimeResolution, std::ratio<1>>(start_time);
  auto duration_sec = std::chrono::duration<TimeResolution, std::ratio<1>>(duration);
  all_stream  << R"({"name":")" << event_name << "\","
              << R"("cat":")" << category << "\","
              << "\"pid\":" << pid << ","
              << "\"tid\":" << tid << ","
              << "\"ts\":" <<  std::chrono::duration_cast<std::chrono::microseconds>(start_sec).count() << ","
              << "\"dur\":" << std::chrono::duration_cast<std::chrono::microseconds>(duration_sec).count() << ","
              << R"("ph":"X",)"
              << R"("args":{)";
  all_stream << "\"hostname\":\"" << hostname() << "\",";
  all_stream << "\"core_affinity\": [";
  for(auto item : core_affinity()) {
      all_stream << item << ",";
  }

  all_stream << "],";
  for(auto item : metadata) {
    if (item.second.type() == typeid(int)) {
      all_stream << "\"" << item.first << "\":" << std::any_cast<int>(item.second) << ",";
    }
  }
  all_stream << "}}\n";
  DLIO_PROFILER_LOGINFO("event logged %s", all_stream.str().c_str());
  return all_stream.str();
}
