//
// Created by haridev on 10/27/23.
//

#ifndef DFTRACER_CONFIGURATION_MANAGER_H
#define DFTRACER_CONFIGURATION_MANAGER_H
#include <cpp-logger/logger.h>
#include <dftracer/core/enumeration.h>

#include <vector>
namespace dftracer {
class ConfigurationManager {
 public:
  bool enable;
  ProfileInitType init_type;
  std::string log_file;
  std::string data_dirs;
  bool metadata;
  bool core_affinity;
  int gotcha_priority;
  cpplogger::LoggerType logger_level;
  bool io;
  bool posix;
  bool stdio;
  bool compression;
  bool trace_all_files;
  bool tids;
  bool bind_signals;
  bool throw_error;
  size_t write_buffer_size;
  ConfigurationManager();
  void finalize() {}
};
}  // namespace dftracer
#endif  // DFTRACER_CONFIGURATION_MANAGER_H
