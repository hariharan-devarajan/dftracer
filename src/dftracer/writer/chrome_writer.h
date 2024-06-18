//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_CHROME_WRITER_H
#define DFTRACER_CHROME_WRITER_H

#include <dftracer/core/constants.h>
#include <dftracer/core/typedef.h>
#include <dftracer/utils/configuration_manager.h>
#include <dftracer/utils/posix_internal.h>
#include <dftracer/utils/utils.h>
#if DISABLE_HWLOC == 1
#include <hwloc.h>
#endif
#include <assert.h>
#include <unistd.h>

#include <any>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#define ERROR(cond, format, ...)          \
  DFTRACER_LOGERROR(format, __VA_ARGS__); \
  if (this->throw_error) assert(cond);
namespace dftracer {
class ChromeWriter {
 private:
  std::unordered_map<char *, std::any> metadata;

 protected:
  bool throw_error;
  std::string filename;

 private:
  bool include_metadata, enable_compression;

  bool enable_core_affinity;
#if DISABLE_HWLOC == 1
  hwloc_topology_t topology;
#endif
  FILE *fh;
  char hostname[256];
  static const int MAX_LINE_SIZE = 4096;
  static const int MAX_META_LINE_SIZE = 3000;
  void convert_json(int index, ConstEventType event_name,
                    ConstEventType category, TimeResolution start_time,
                    TimeResolution duration,
                    std::unordered_map<std::string, std::any> *metadata,
                    ProcessID process_id, ThreadID thread_id, int *size,
                    char *data);

  bool is_first_write;
  inline size_t write_buffer_op(const char *write_buffer,
                                const size_t write_size) {
    DFTRACER_LOGDEBUG("ChromeWriter.write_buffer_op %s writing size %d",
                      this->filename.c_str(), write_size);
    flockfile(fh);
    auto written_elements = fwrite(write_buffer, sizeof(char), write_size, fh);
    funlockfile(fh);
    if (written_elements != write_size) {  // GCOVR_EXCL_START
      ERROR(written_elements != write_size,
            "unable to log write %s for a+ written only %d of %d with error %s",
            filename.c_str(), written_elements, write_size, strerror(errno));
    }  // GCOVR_EXCL_STOP
    return written_elements;
  }
  std::vector<unsigned> core_affinity() {
    DFTRACER_LOGDEBUG("ChromeWriter.core_affinity", "");
    auto cores = std::vector<unsigned>();
#if DISABLE_HWLOC == 1
    if (enable_core_affinity) {
      hwloc_cpuset_t set = hwloc_bitmap_alloc();
      hwloc_get_cpubind(topology, set, HWLOC_CPUBIND_PROCESS);
      for (unsigned id = hwloc_bitmap_first(set); id != -1;
           id = hwloc_bitmap_next(set, id)) {
        cores.push_back(id);
      }
      hwloc_bitmap_free(set);
    }
#endif
    return cores;
  }

  void get_hostname(char *hostname) {
    DFTRACER_LOGDEBUG("ChromeWriter.get_hostname", "");
    gethostname(hostname, 256);
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
        is_first_write(true) {
    DFTRACER_LOGDEBUG("ChromeWriter.ChromeWriter", "");
    auto conf =
        dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
    get_hostname(hostname);
    include_metadata = conf->metadata;
    enable_core_affinity = conf->core_affinity;
    enable_compression = conf->compression;
    if (enable_core_affinity) {
#if DISABLE_HWLOC == 1
      hwloc_topology_init(&topology);  // initialization
      hwloc_topology_load(topology);   // actual detection
#endif
    }
  }
  ~ChromeWriter() { DFTRACER_LOGDEBUG("Destructing ChromeWriter", ""); }
  void initialize(char *filename, bool throw_error);

  void log(int index, ConstEventType event_name, ConstEventType category,
           TimeResolution &start_time, TimeResolution &duration,
           std::unordered_map<std::string, std::any> *metadata,
           ProcessID process_id, ThreadID tid);

  void finalize(bool has_entry);
};
}  // namespace dftracer

#endif  // DFTRACER_CHROME_WRITER_H
