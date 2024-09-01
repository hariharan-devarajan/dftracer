//
// Created by haridev on 3/28/23.
//

#include <dftracer/core/logging.h>
#include <dftracer/writer/chrome_writer.h>
#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <thread>

template <>
std::shared_ptr<dftracer::ChromeWriter>
    dftracer::Singleton<dftracer::ChromeWriter>::instance = nullptr;
template <>
bool dftracer::Singleton<dftracer::ChromeWriter>::stop_creating_instances =
    false;
void dftracer::ChromeWriter::initialize(char *filename, bool throw_error) {
  this->throw_error = throw_error;
  this->filename = filename;
  if (fh == nullptr) {
    fh = fopen(filename, "ab+");
    if (fh == nullptr) {
      DFTRACER_LOG_ERROR("unable to create log file %s",
                         filename);  // GCOVR_EXCL_LINE
    } else {
      setvbuf(fh, NULL, _IOFBF, MAX_BUFFER);
      DFTRACER_LOG_INFO("created log file %s", filename);
    }
  }
  DFTRACER_LOG_DEBUG("ChromeWriter.initialize %s", this->filename.c_str());
}

void dftracer::ChromeWriter::log(
    int index, ConstEventType event_name, ConstEventType category,
    TimeResolution &start_time, TimeResolution &duration,
    std::unordered_map<std::string, std::any> *metadata, ProcessID process_id,
    ThreadID thread_id) {
  DFTRACER_LOG_DEBUG("ChromeWriter.log", "");

  if (fh != nullptr) {
    convert_json(index, event_name, category, start_time, duration, metadata,
                 process_id, thread_id);
    write_buffer_op();
  } else {
    DFTRACER_LOG_ERROR("ChromeWriter.log invalid", "");
  }
  is_first_write = false;
}

void dftracer::ChromeWriter::finalize(bool has_entry) {
  DFTRACER_LOG_DEBUG("ChromeWriter.finalize", "");
  if (fh != nullptr) {
    DFTRACER_LOG_INFO("Profiler finalizing writer %s", filename.c_str());
    write_buffer_op(true);
    fflush(fh);
    int last_off = ftell(fh);
    (void)last_off;
    int status = fclose(fh);
    if (status != 0) {
      DFTRACER_LOG_ERROR("unable to close log file %s for a+",
                         filename.c_str());  // GCOVR_EXCL_LINE
    }
    if (!has_entry) {
      DFTRACER_LOG_INFO(
          "No trace data written as offset is %d. Deleting file %s", last_off,
          filename.c_str());
      df_unlink(filename.c_str());
    } else {
      DFTRACER_LOG_INFO("Profiler writing the final symbol", "");
      fh = fopen(this->filename.c_str(), "r+");
      if (fh == nullptr) {
        DFTRACER_LOG_ERROR("unable to open log file %s with O_WRONLY",
                           this->filename.c_str());  // GCOVR_EXCL_LINE
      } else {
        std::string data = "[\n";
        auto written_elements =
            fwrite(data.c_str(), sizeof(char), data.size(), fh);
        if (written_elements != data.size()) {  // GCOVR_EXCL_START
          DFTRACER_LOG_ERROR(
              "unable to finalize log write %s for O_WRONLY written only %ld "
              "of %ld",
              filename.c_str(), data.size(), written_elements);
        }  // GCOVR_EXCL_STOP
        status = fclose(fh);
        if (status != 0) {
          DFTRACER_LOG_ERROR("unable to close log file %s for O_WRONLY",
                             filename.c_str());  // GCOVR_EXCL_LINE
        }
      }
      if (enable_compression) {
        if (system("which gzip > /dev/null 2>&1")) {
          DFTRACER_LOG_ERROR("Gzip compression does not exists",
                             "");  // GCOVR_EXCL_LINE
        } else {
          DFTRACER_LOG_INFO("Applying Gzip compression on file %s",
                            filename.c_str());
          char cmd[2048];
          sprintf(cmd, "gzip -f %s", filename.c_str());
          int ret = system(cmd);
          if (ret == 0) {
            DFTRACER_LOG_INFO("Successfully compressed file %s.gz",
                              filename.c_str());
          } else {
            DFTRACER_LOG_ERROR("Unable to compress file %s", filename.c_str());
          }
        }
      }
    }
  }
  if (enable_core_affinity) {
#if DISABLE_HWLOC == 1
    hwloc_topology_destroy(topology);
#endif
  }

  {
    std::unique_lock<std::shared_mutex> lock(mtx);
    if (buffer) {
      free(buffer);
      current_index = 0;
    }
  }

  DFTRACER_LOG_DEBUG("Finished writer finalization", "");
}

void dftracer::ChromeWriter::convert_json(
    int index, ConstEventType event_name, ConstEventType category,
    TimeResolution start_time, TimeResolution duration,
    std::unordered_map<std::string, std::any> *metadata, ProcessID process_id,
    ThreadID thread_id) {
  auto previous_index = current_index;
  (void)previous_index;
  char is_first_char[3] = "  ";
  if (!is_first_write) is_first_char[0] = '\0';
  if (include_metadata) {
    std::stringstream all_stream;
    auto cores = core_affinity();
    auto cores_size = cores.size();
    if (cores_size > 0) {
      all_stream << ", \"core_affinity\": [";
      for (long unsigned int i = 0; i < cores_size; ++i) {
        all_stream << cores[i];
        if (i < cores_size - 1) all_stream << ",";
      }
      all_stream << "]";
    }
    bool has_meta = false;
    std::stringstream meta_stream;
    auto meta_size = metadata->size();
    long unsigned int i = 0;
    for (auto item : *metadata) {
      has_meta = true;
      if (item.second.type() == typeid(unsigned int)) {
        meta_stream << "\"" << item.first
                    << "\":" << std::any_cast<unsigned int>(item.second);
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(int)) {
        meta_stream << "\"" << item.first
                    << "\":" << std::any_cast<int>(item.second);
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(const char *)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<const char *>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(std::string)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<std::string>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(size_t)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<size_t>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(long)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<long>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(ssize_t)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<ssize_t>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(off_t)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<off_t>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else if (item.second.type() == typeid(off64_t)) {
        meta_stream << "\"" << item.first << "\":\""
                    << std::any_cast<off64_t>(item.second) << "\"";
        if (i < meta_size - 1) meta_stream << ",";
      } else {
        DFTRACER_LOG_INFO("No conversion for type %s", item.first.c_str());
      }
      i++;
    }
    if (has_meta) {
      all_stream << "," << meta_stream.str();
    }
    {
      std::unique_lock<std::shared_mutex> lock(mtx);
      auto written_size = sprintf(
          buffer + current_index,
          R"(%s{"id":%d,"name":"%s","cat":"%s","pid":%lu,"tid":%lu,"ts":%llu,"dur":%llu,"ph":"X","args":{"hostname":"%s"%s}})",
          is_first_char, index, event_name, category, process_id, thread_id,
          start_time, duration, this->hostname, all_stream.str().c_str());
      current_index += written_size;
      buffer[current_index] = '\n';
      current_index++;
    }
  } else {
    {
      std::unique_lock<std::shared_mutex> lock(mtx);
      auto written_size = sprintf(
          buffer + current_index,
          R"(%s{"id":%d,"name":"%s","cat":"%s","pid":%lu,"tid":%lu,"ts":%llu,"dur":%llu,"ph":"X"})",
          is_first_char, index, event_name, category, process_id, thread_id,
          start_time, duration);
      current_index += written_size;
      buffer[current_index] = '\n';
      current_index++;
    }
  }
  DFTRACER_LOG_DEBUG("ChromeWriter.convert_json %s on %s",
                     buffer + previous_index, this->filename.c_str());
}
