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
namespace dlio_profiler {
    class ChromeWriter: public BaseWriter {
    private:
        hwloc_topology_t topology;
        FILE* fp;
        std::string convert_json(std::string &event_name, std::string &category, TimeResolution start_time, TimeResolution duration,
                                 std::unordered_map<std::string, std::any> &metadata, int process_id);
        bool is_first_write;
        int process_id;
        std::unordered_map<int, std::mutex> mtx_map;
        std::vector<unsigned> core_affinity() {
          auto cores = std::vector<unsigned>();
          hwloc_cpuset_t set = hwloc_bitmap_alloc();
          hwloc_get_cpubind(topology, set, HWLOC_CPUBIND_PROCESS);
          for (unsigned id = hwloc_bitmap_first(set);  id != -1;  id = hwloc_bitmap_next(set, id)) {
            cores.push_back(id);
          }
          hwloc_bitmap_free(set);
          return cores;
        }
        std::string hostname() {
          const int SIZE=256;
          char hostname[SIZE];
          gethostname(hostname, SIZE);
          return std::string(hostname);
        }
    public:
        ChromeWriter(FILE* fp=nullptr):BaseWriter(), is_first_write(true), mtx_map(){
          process_id = getpid();
          this->fp = fp;
          hwloc_topology_init(&topology);  // initialization
          hwloc_topology_load(topology);   // actual detection
        }
        void initialize(char *filename, bool throw_error) override;

        void log(std::string &event_name, std::string &category, TimeResolution &start_time, TimeResolution &duration,
                 std::unordered_map<std::string , std::any> &metadata, int process_id) override;

        void finalize() override;
    };
}

#endif //DLIO_PROFILER_CHROME_WRITER_H
