//
// Created by haridev on 3/28/23.
//

#include <dlio_profiler/dlio_profiler.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include<algorithm>
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/utils/utils.h>
#include <dlio_profiler/core/constants.h>
#include <dlio_profiler/core/dlio_profiler_main.h>

namespace dlio_profiler {
    bool init = false;
}

bool is_init() {return dlio_profiler::init;}
void set_init(bool _init) { dlio_profiler::init = _init;}
void dlio_profiler_init(void) {
  char *init_type_count = getenv(DLIO_PROFILER_INIT_COUNT);
  char *init_type = getenv(DLIO_PROFILER_INIT);
  if (init_type_count == nullptr && init_type != nullptr && strcmp(init_type, "PRELOAD") == 0) {
    int pid = getpid();
    dlio_profiler::Singleton<dlio_profiler::DLIOProfiler>::get_instance(true, true, nullptr, nullptr, &pid);
    DLIO_PROFILER_LOGINFO("Running initialize within constructor %d", getpid());
    set_init(true);
    int val = setenv(DLIO_PROFILER_INIT_COUNT, "1", 1);
    (void) val;
  }

}
void dlio_profiler_fini(void) {
  char *init_type = getenv(DLIO_PROFILER_INIT);
  char *init_type_count = getenv(DLIO_PROFILER_INIT_COUNT);
  if (init_type_count != nullptr && init_type != nullptr && strcmp(init_type, "PRELOAD")) {
    dlio_profiler::Singleton<dlio_profiler::DLIOProfiler>::get_instance(false, false)->finalize();
    set_init(false);
    int val = unsetenv(DLIO_PROFILER_INIT_COUNT);
    (void) val;
  }
}