//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_CHROME_WRITER_H
#define DLIO_PROFILER_CHROME_WRITER_H

#include <dlio_profiler/writer/base_writer.h>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <hwloc.h>
#include <dlio_profiler/core/constants.h>
#include <atomic>

namespace dlio_profiler {
    class ChromeWriter : public BaseWriter {
    private:
        bool enable_core_affinity, include_metadata, enable_compression;
        hwloc_topology_t topology;
        int fd;
        std::atomic_int index;

        std::string
        convert_json(std::string &event_name, std::string &category, TimeResolution start_time, TimeResolution duration,
                     std::unordered_map<std::string, std::any> &metadata, int process_id, int thread_id);

        bool is_first_write;
        int process_id;
        std::unordered_map<int, std::mutex> mtx_map;

        std::vector<unsigned> core_affinity() {
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

        std::string hostname() {
          const int SIZE = 256;
          char hostname[SIZE];
          gethostname(hostname, SIZE);
          return std::string(hostname);
        }

    public:
        ChromeWriter(int fd = -1)
                : BaseWriter(), is_first_write(true), mtx_map(), enable_core_affinity(false), include_metadata(false),
                  enable_compression(false), index(0){
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
          process_id = dlp_getpid();
          this->fd = fd;
          if (enable_core_affinity) {
            hwloc_topology_init(&topology);  // initialization
            hwloc_topology_load(topology);   // actual detection
          }
        }

        void initialize(char *filename, bool throw_error) override;

        void log(std::string &event_name, std::string &category, TimeResolution &start_time, TimeResolution &duration,
                 std::unordered_map<std::string, std::any> &metadata, int process_id, int tid) override;

        void finalize() override;
    };
}

#endif //DLIO_PROFILER_CHROME_WRITER_H
