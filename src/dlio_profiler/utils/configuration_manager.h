//
// Created by haridev on 10/27/23.
//

#ifndef DLIO_PROFILER_CONFIGURATION_MANAGER_H
#define DLIO_PROFILER_CONFIGURATION_MANAGER_H
#include <cpp-logger/logger.h>
#include <dlio_profiler/core/enumeration.h>

#include <vector>
namespace dlio_profiler {
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
} //dlio_profiler
#endif  // DLIO_PROFILER_CONFIGURATION_MANAGER_H
