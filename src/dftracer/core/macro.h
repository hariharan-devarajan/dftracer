//
// Created by hariharan on 8/16/22.
//

#ifndef DFTRACER_MACRO_H
#define DFTRACER_MACRO_H

#include <cpp-logger/logger.h>

#define DFTRACER_LOGGER cpplogger::Logger::Instance("DFTRACER")
#define DFTRACER_LOGDEBUG(format, ...) \
  DFTRACER_LOGGER->log(cpplogger::LOG_DEBUG, format, __VA_ARGS__);
#define DFTRACER_LOGINFO(format, ...) \
  DFTRACER_LOGGER->log(cpplogger::LOG_INFO, format, __VA_ARGS__);
#define DFTRACER_LOGWARN(format, ...) \
  DFTRACER_LOGGER->log(cpplogger::LOG_WARN, format, __VA_ARGS__);
#define DFTRACER_LOGERROR(format, ...) \
  DFTRACER_LOGGER->log(cpplogger::LOG_ERROR, format, __VA_ARGS__);
#define DFTRACER_LOGPRINT(format, ...) \
  DFTRACER_LOGGER->log(cpplogger::LOG_PRINT, format, __VA_ARGS__);
#endif  // DFTRACER_MACRO_H