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
#include <execinfo.h>
#include <dlio_profiler/core/singleton.h>
#include <dlio_profiler/core/enumeration.h>
#include <dlio_profiler/core/error.h>
#include <any>
#include <csignal>
#include "typedef.h"
#include <dlio_profiler/core/typedef.h>
#include <dlio_profiler/dlio_logger.h>

namespace dlio_profiler {
    class DLIOProfilerCore {
    private:
        bool is_enabled;
        int gotcha_priority;
        cpplogger::LoggerType logger_level;
        std::string log_file;
        std::string data_dirs;
        ProcessID process_id;
        bool is_initialized;
        bool bind;
        bool enable_posix;
        bool enable_stdio;
        bool enable_io;
        std::shared_ptr<DLIOLogger> logger;

        void initlialize(bool is_init, bool _bind, const char *_log_file = nullptr, const char *_data_dirs = nullptr,
                         const int *_process_id = nullptr);

    public:
        DLIOProfilerCore(ProfilerStage stage, ProfileType type, const char *log_file = nullptr,
                         const char *data_dirs = nullptr, const int *process_id = nullptr);

        inline bool is_active() {
          return is_enabled;
        }

        TimeResolution get_time();

        void log(const char *event_name, const char *category,
                 TimeResolution start_time, TimeResolution duration,
                 std::unordered_map<std::string, std::any> &metadata);

        bool finalize();
    };
}  // namespace dlio_profiler

#define DLIO_PROFILER_MAIN_SINGLETON_INIT(stage, type, ...) \
dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(stage, type, __VA_ARGS__)

#define DLIO_PROFILER_MAIN_SINGLETON(stage, type) \
dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(stage, type)
#endif //DLIO_PROFILER_DLIO_PROFILER_MAIN_H
