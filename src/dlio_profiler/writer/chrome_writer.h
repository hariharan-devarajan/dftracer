//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_CHROME_WRITER_H
#define DLIO_PROFILER_CHROME_WRITER_H

#include <dlio_profiler/core/constants.h>
#include <dlio_profiler/core/typedef.h>
#include <dlio_profiler/utils/posix_internal.h>
#include <dlio_profiler/utils/utils.h>
#include <hwloc.h>
#include <unistd.h>

#include <any>
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#define ERROR(cond, format, ...) \
  DLIO_PROFILER_LOGERROR(format, __VA_ARGS__); \
  if (this->throw_error) assert(cond);
namespace dlio_profiler {
    class ChromeWriter {
    private:
        std::unordered_map<char *, std::any> metadata;
    protected:
        bool throw_error;
        std::string filename;
    private:
        bool enable_core_affinity, include_metadata, enable_compression;
        hwloc_topology_t topology;
        int fd;
        std::atomic_int index;
        char hostname[256];
        static const int MAX_LINE_SIZE=4096;
        static const int MAX_META_LINE_SIZE=3000;
        void convert_json(ConstEventType event_name, ConstEventType category, TimeResolution start_time,
                                                  TimeResolution duration, std::unordered_map<std::string, std::any> *metadata,
                                                  ProcessID process_id, ThreadID thread_id, int* size, char* data);

        bool is_first_write;
        static const int WRITE_BUFFER_SIZE=1024*1024;
        size_t write_size;
        char* write_buffer;
        inline int write_buffer_op(){
          DLIO_PROFILER_LOGDEBUG("ChromeWriter.write_buffer_op","");
          auto written_elements = dlp_write(fd, write_buffer, write_size);
          if (written_elements != write_size) {  // GCOVR_EXCL_START
            ERROR(written_elements != write_size, "unable to log write %s fd %d for a+ written only %d of %d with error %s",
                  filename.c_str(), fd, written_elements, write_size, strerror(errno));
          }  // GCOVR_EXCL_STOP
          return write_size;
        }
        inline int merge_buffer(const char* data, int size);

        std::vector<unsigned> core_affinity() {
          DLIO_PROFILER_LOGDEBUG("ChromeWriter.core_affinity","");
          auto cores = std::vector<unsigned>();
          if (enable_core_affinity) {
            hwloc_cpuset_t set = hwloc_bitmap_alloc();
            hwloc_get_cpubind(topology, set, HWLOC_CPUBIND_PROCESS);
            for (unsigned id = hwloc_bitmap_first(set); id != -1; id = hwloc_bitmap_next(set, id)) {
              cores.push_back(id);
            }
            hwloc_bitmap_free(set);
          }
          return cores;
        }

        void get_hostname(char* hostname) {
          DLIO_PROFILER_LOGDEBUG("ChromeWriter.get_hostname","");
          gethostname(hostname, 256);
        }

    public:
        ChromeWriter(int fd = -1): is_first_write(true), enable_core_affinity(false), include_metadata(false),
                  enable_compression(false), index(0), write_size(0){
          DLIO_PROFILER_LOGDEBUG("ChromeWriter.ChromeWriter","");
          write_buffer = static_cast<char *>(malloc(WRITE_BUFFER_SIZE + MAX_LINE_SIZE));
          get_hostname(hostname);
          char *dlio_profiler_meta = getenv(DLIO_PROFILER_INC_METADATA);
          if (dlio_profiler_meta != nullptr && strcmp(dlio_profiler_meta, "1") == 0) {
            include_metadata = true;
          }
          char *enable_core_affinity_str = getenv(DLIO_PROFILER_SET_CORE_AFFINITY);
          if (enable_core_affinity_str != nullptr && strcmp(enable_core_affinity_str, "1") == 0) {
            enable_core_affinity = true;
          }
          char *enable_compression_str = getenv(DLIO_PROFILER_TRACE_COMPRESSION);
          if (enable_compression_str != nullptr && strcmp(enable_compression_str, "1") == 0) {
            enable_compression = true;
          }
          this->fd = fd;
          if (enable_core_affinity) {
            hwloc_topology_init(&topology);  // initialization
            hwloc_topology_load(topology);   // actual detection
          }
        }
        ~ChromeWriter(){DLIO_PROFILER_LOGDEBUG("Destructing ChromeWriter","");}
        void initialize(char *filename, bool throw_error);

        void log(ConstEventType event_name, ConstEventType category, TimeResolution &start_time, TimeResolution &duration,
                 std::unordered_map<std::string, std::any> *metadata, ProcessID process_id, ThreadID tid);

        void finalize();
    };
}

#endif //DLIO_PROFILER_CHROME_WRITER_H
