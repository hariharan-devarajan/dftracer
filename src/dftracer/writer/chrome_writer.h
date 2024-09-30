//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_CHROME_WRITER_H
#define DFTRACER_CHROME_WRITER_H

#include <assert.h>
#include <dftracer/core/constants.h>
#include <dftracer/core/typedef.h>
#include <dftracer/utils/configuration_manager.h>
#include <dftracer/utils/posix_internal.h>
#include <dftracer/utils/utils.h>
#include <unistd.h>

#include <any>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
namespace dftracer {
class ChromeWriter {
 private:
  std::unordered_map<char *, std::any> metadata;
  std::shared_mutex mtx;

 protected:
  bool throw_error;
  std::string filename;

 private:
  bool include_metadata, enable_compression;

  bool enable_core_affinity;

  FILE *fh;
  uint16_t hostname_hash;
  static const int MAX_LINE_SIZE = 16*1024L;
  size_t write_buffer_size;

  size_t current_index;
  std::vector<char> buffer;
  void convert_json(int index, ConstEventNameType event_name,
                    ConstEventNameType category, TimeResolution start_time,
                    TimeResolution duration,
                    std::unordered_map<std::string, std::any> *metadata,
                    ProcessID process_id, ThreadID thread_id);

  void convert_json_metadata(int index, ConstEventNameType name,
                             ConstEventNameType value, ConstEventNameType ph,
                             ProcessID process_id, ThreadID thread_id,
                             bool is_string);

  bool is_first_write;
  inline size_t write_buffer_op(bool force = false) {
    if (current_index == 0 || (!force && current_index < write_buffer_size))
      return 0;
    DFTRACER_LOG_DEBUG("ChromeWriter.write_buffer_op %s",
                       this->filename.c_str());
    size_t written_elements = 0;
    {
      std::unique_lock<std::shared_mutex> lock(mtx);
      flockfile(fh);
      written_elements = fwrite(buffer.data(), current_index, sizeof(char), fh);
      current_index = 0;
      funlockfile(fh);
    }

    if (written_elements != 1) {  // GCOVR_EXCL_START
      DFTRACER_LOG_ERROR(
          "unable to log write only %ld of %d with error code "
          "%d",
          written_elements, 1, errno);
    }  // GCOVR_EXCL_STOP
    return written_elements;
  }

 public:
  ChromeWriter()
      : metadata(),
        throw_error(false),
        filename(),
        include_metadata(false),
        enable_compression(false),
        enable_core_affinity(false),
        fh(nullptr),
        current_index(0),
        is_first_write(true) {
    DFTRACER_LOG_DEBUG("ChromeWriter.ChromeWriter", "");
    auto conf =
        dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
    include_metadata = conf->metadata;
    enable_core_affinity = conf->core_affinity;
    enable_compression = conf->compression;
    write_buffer_size = conf->write_buffer_size;
    {
      std::unique_lock<std::shared_mutex> lock(mtx);
      buffer = std::vector<char>(write_buffer_size + MAX_LINE_SIZE);
      current_index = 0;
    }
  }
  ~ChromeWriter() { DFTRACER_LOG_DEBUG("Destructing ChromeWriter", ""); }
  void initialize(char *filename, bool throw_error, uint16_t hostname_hash);

  void log(int index, ConstEventNameType event_name,
           ConstEventNameType category, TimeResolution start_time,
           TimeResolution duration,
           std::unordered_map<std::string, std::any> *metadata,
           ProcessID process_id, ThreadID tid);

  void log_metadata(int index, ConstEventNameType name,
                    ConstEventNameType value, ConstEventNameType ph, ProcessID process_id,
                    ThreadID tid, bool is_string = true);

  void finalize(bool has_entry);
};
}  // namespace dftracer

#endif  // DFTRACER_CHROME_WRITER_H
