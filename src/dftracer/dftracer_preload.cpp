//
// Created by haridev on 3/28/23.
//

#include <dftracer/core/dftracer_main.h>
#include <dftracer/df_logger.h>
#include <dftracer/dftracer_preload.h>
#include <dftracer/utils/configuration_manager.h>

#include <algorithm>

namespace dftracer {
bool init = false;
}

bool is_init() { return dftracer::init; }

void set_init(bool _init) { dftracer::init = _init; }

void dftracer_init(void) {
  auto conf =
      dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
  DFTRACER_LOGDEBUG("dftracer_init", "");
  if (!is_init()) {
    dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
        ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_PRELOAD);
    set_init(true);
  }
}

void dftracer_fini(void) {
  auto conf =
      dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
  DFTRACER_LOGDEBUG("dftracer_fini", "");
  if (is_init()) {
    auto dftracer_inst =
        dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
            ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_PRELOAD);
    if (dftracer_inst != nullptr) {
      dftracer_inst->finalize();
    }
    set_init(false);
  }
}