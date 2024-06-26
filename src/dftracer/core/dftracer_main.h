//
// Created by haridev on 10/5/23.
//

#ifndef DFTRACER_DFTRACER_MAIN_H
#define DFTRACER_DFTRACER_MAIN_H

#include <brahma/brahma.h>
#include <cpp-logger/logger.h>
#include <dftracer/brahma/posix.h>
#include <dftracer/brahma/stdio.h>
#include <dftracer/core/constants.h>
#include <dftracer/core/enumeration.h>
#include <dftracer/core/error.h>
#include <dftracer/core/macro.h>
#include <dftracer/core/singleton.h>
#include <dftracer/core/typedef.h>
#include <dftracer/df_logger.h>
#include <execinfo.h>

#include <any>
#include <csignal>
#include <cstring>
#include <stdexcept>
#include <thread>

namespace dftracer {
class DFTracerCore {
 private:
  std::string log_file;
  std::string data_dirs;
  std::shared_ptr<dftracer::ConfigurationManager> conf;
  ProcessID process_id;
  bool is_initialized;
  bool bind;
  std::string log_file_suffix;
  std::shared_ptr<DFTLogger> logger;
  void initialize(bool _bind, const char *_log_file = nullptr,
                  const char *_data_dirs = nullptr,
                  const int *_process_id = nullptr);

 public:
  bool include_metadata;
  DFTracerCore(ProfilerStage stage, ProfileType type,
               const char *log_file = nullptr, const char *data_dirs = nullptr,
               const int *process_id = nullptr);

  inline bool is_active() {
    DFTRACER_LOGDEBUG("DFTracerCore.is_active", "");
    return conf->enable;
  }

  TimeResolution get_time();

  void log(ConstEventType event_name, ConstEventType category,
           TimeResolution start_time, TimeResolution duration,
           std::unordered_map<std::string, std::any> *metadata);

  inline void enter_event() { logger->enter_event(); }

  inline void exit_event() { logger->exit_event(); }

  bool finalize();
  ~DFTracerCore() { DFTRACER_LOGDEBUG("Destructing DFTracerCore", ""); }
};
}  // namespace dftracer

#define DFTRACER_MAIN_SINGLETON_INIT(stage, type, ...)                   \
  dftracer::Singleton<dftracer::DFTracerCore>::get_instance(stage, type, \
                                                            __VA_ARGS__)

#define DFTRACER_MAIN_SINGLETON(stage, type) \
  dftracer::Singleton<dftracer::DFTracerCore>::get_instance(stage, type)
#endif  // DFTRACER_DFTRACER_MAIN_H
