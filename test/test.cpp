//
// Created by hariharan on 8/8/22.
//


#include <hwloc.h>
#include <string>
int main(int argc, char* argv[]) {
  hwloc_topology_t topology;
  hwloc_topology_init(&topology);  // initialization
  hwloc_topology_load(topology);   // actual detection
  hwloc_cpuset_t set = hwloc_bitmap_alloc();
  hwloc_get_cpubind(topology, set, HWLOC_CPUBIND_PROCESS);
  for (unsigned id = hwloc_bitmap_first(set);  id != -1;  id = hwloc_bitmap_next(set, id)) {
    printf("%d", id);
  }
  return 0;
}