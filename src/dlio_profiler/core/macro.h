//
// Created by hariharan on 8/16/22.
//

#ifndef DLIO_PROFILER_MACRO_H
#define DLIO_PROFILER_MACRO_H

#include <cpp-logger/logger.h>

#define DLIO_PROFILER_LOGGER cpplogger::Logger::Instance("DLIO_PROFILER")
#define DLIO_PROFILER_LOGDEBUG(format, ...) \
  DLIO_PROFILER_LOGGER->log(cpplogger::LOG_DEBUG, format, __VA_ARGS__);
#define DLIO_PROFILER_LOGINFO(format, ...) \
  DLIO_PROFILER_LOGGER->log(cpplogger::LOG_INFO, format, __VA_ARGS__);
#define DLIO_PROFILER_LOGWARN(format, ...) \
  DLIO_PROFILER_LOGGER->log(cpplogger::LOG_WARN, format, __VA_ARGS__);
#define DLIO_PROFILER_LOGERROR(format, ...) \
  DLIO_PROFILER_LOGGER->log(cpplogger::LOG_ERROR, format, __VA_ARGS__);
#define DLIO_PROFILER_LOGPRINT(format, ...) \
  DLIO_PROFILER_LOGGER->log(cpplogger::LOG_PRINT, format, __VA_ARGS__);
#endif  // DLIO_PROFILER_MACRO_H