//
// Created by haridev on 10/5/23.
//

#ifndef DLIO_PROFILER_DLIO_PROFILER_MAIN_H
#define DLIO_PROFILER_DLIO_PROFILER_MAIN_H

#include <cstring>
#include <thread>
#include <stdexcept>

#include <cpp-logger/logger.h>
#include <dlio_profiler/core/constants.h>
#include <dlio_profiler/core/macro.h>
#include <brahma/brahma.h>
#include <dlio_profiler/brahma/posix.h>
#include <dlio_profiler/brahma/stdio.h>
#include <dlio_profiler/dlio_logger.h>
#include <execinfo.h>
#include <dlio_profiler/core/singleton.h>
#include <dlio_profiler/core/enumeration.h>
#include <dlio_profiler/core/error.h>
#include <any>
#include <csignal>
#include <dlio_profiler/core/typedef.h>
#include <dlio_profiler/dlio_logger.h>

namespace dlio_profiler {
    class DLIOProfilerCore {
    private:
        std::string log_file;
        std::string data_dirs;
        std::shared_ptr<dlio_profiler::ConfigurationManager> conf;
        ProcessID process_id;
        bool is_initialized;
        bool bind;
        std::shared_ptr<DLIOLogger> logger;
        void initialize(bool _bind, const char *_log_file = nullptr, const char *_data_dirs = nullptr,
                         const int *_process_id = nullptr);

    public:
        bool include_metadata;
        DLIOProfilerCore(ProfilerStage stage, ProfileType type, const char *log_file = nullptr,
                         const char *data_dirs = nullptr, const int *process_id = nullptr);

        inline bool is_active() {
          DLIO_PROFILER_LOGDEBUG("DLIOProfilerCore.is_active","");
          return conf->enable;
        }

        TimeResolution get_time();

        void log(ConstEventType event_name, ConstEventType category,
                 TimeResolution start_time, TimeResolution duration,
                 std::unordered_map<std::string, std::any> *metadata);

        bool finalize();
        ~DLIOProfilerCore(){
          DLIO_PROFILER_LOGDEBUG("Destructing DLIOProfilerCore","");
        }
    };
}  // namespace dlio_profiler

#define DLIO_PROFILER_MAIN_SINGLETON_INIT(stage, type, ...) \
dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(stage, type, __VA_ARGS__)

#define DLIO_PROFILER_MAIN_SINGLETON(stage, type) \
dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(stage, type)
#endif //DLIO_PROFILER_DLIO_PROFILER_MAIN_H
