//
// Created by haridev on 3/28/23.
//

#include <dlio_profiler/dlio_profiler_preload.h>
#include<algorithm>
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/core/dlio_profiler_main.h>

namespace dlio_profiler {
    bool init = false;
}

bool is_init() {return dlio_profiler::init;}
void set_init(bool _init) { dlio_profiler::init = _init;}
void dlio_profiler_init(void) {
    dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_PRELOAD);
}
void dlio_profiler_fini(void) {
    dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_PRELOAD)->finalize();

}