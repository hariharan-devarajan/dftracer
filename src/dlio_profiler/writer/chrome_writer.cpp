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

#define ERROR(cond, format, ...) \
  DLIO_PROFILER_LOGERROR(format, __VA_ARGS__); \
  if (this->throw_error) assert(cond);
void dlio_profiler::ChromeWriter::initialize(char *filename, bool throw_error) {
  this->throw_error = throw_error;
  this->filename = filename;
}

void
dlio_profiler::ChromeWriter::log(std::string &event_name, std::string &category, double &start_time, double &duration,
                                 std::unordered_map<std::string, std::any> &metadata) {
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
    file_mtx.lock();
    std::string json = convert_json(event_name, category, start_time, duration, metadata);
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
dlio_profiler::ChromeWriter::convert_json(std::string &event_name, std::string &category, double &start_time,
                                          double &duration, std::unordered_map<std::string, std::any> &metadata) {
  std::stringstream all_stream;
  auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id()) % 100000;
  all_stream  << R"({"name":")" << event_name << "\","
              << R"("cat":")" << category << "\","
              << "\"pid\":" << getpid() << ","
              << "\"tid\":" << tid << ","
              << "\"ts\":" << start_time * 1000.0 << ","
              << "\"dur\":" << duration * 1000.0 << ","
              << R"("ph":"X",)"
              << R"("args":{)";
  for(auto item : metadata) {
    if (item.second.type() == typeid(int)) {
      all_stream << "\"" << item.first << "\":" << std::any_cast<int>(item.second) << ",";
    }
  }
  all_stream << "}}\n";
  DLIO_PROFILER_LOGINFO("event logged %s", all_stream.str().c_str());
  return all_stream.str();
}
