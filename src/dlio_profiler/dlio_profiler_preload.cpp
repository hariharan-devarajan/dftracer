//
// Created by haridev on 3/28/23.
//

#include <dlio_profiler/core/dlio_profiler_main.h>
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/dlio_profiler_preload.h>
#include <dlio_profiler/utils/configuration_manager.h>

#include <algorithm>

namespace dlio_profiler {
    bool init = false;
}

bool is_init() { return dlio_profiler::init; }

void set_init(bool _init) { dlio_profiler::init = _init; }

void dlio_profiler_init(void) {
  auto conf = dlio_profiler::Singleton<dlio_profiler::ConfigurationManager>::get_instance();
  DLIO_PROFILER_LOGDEBUG("dlio_profiler_init","");
  if (!is_init()) {
    dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(ProfilerStage::PROFILER_INIT,
                                                                            ProfileType::PROFILER_PRELOAD);
    set_init(true);
  }
}

void dlio_profiler_fini(void) {
  auto conf = dlio_profiler::Singleton<dlio_profiler::ConfigurationManager>::get_instance();
  DLIO_PROFILER_LOGDEBUG("dlio_profiler_fini","");
  if (is_init()) {
    auto dlio_profiler_inst = dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(
            ProfilerStage::PROFILER_FINI,
            ProfileType::PROFILER_PRELOAD);
    if (dlio_profiler_inst != nullptr) {
      dlio_profiler_inst->finalize();
      dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::finalize();
    }
    set_init(false);
  }

}