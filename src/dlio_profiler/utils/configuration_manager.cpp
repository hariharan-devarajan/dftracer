//
// Created by haridev on 10/27/23.
//

#include "configuration_manager.h"

#include <dlio_profiler/core/constants.h>
#include <yaml-cpp/yaml.h>

#include "utils.h"

#define DLP_YAML_ENABLE "enable"
// PROFILER
#define DLP_YAML_PROFILER "profiler"
#define DLP_YAML_PROFILER_INIT "init"
#define DLP_YAML_PROFILER_LOG_FILE "log_file"
#define DLP_YAML_PROFILER_DATA_DIRS "data_dirs"
#define DLP_YAML_PROFILER_LOG_LEVEL "log_level"
#define DLP_YAML_PROFILER_COMPRESSION "compression"
// GOTCHA
#define DLP_YAML_GOTCHA "gotcha"
#define DLP_YAML_GOTCHA_PRIORITY "priority"
// Features
#define DLP_YAML_FEATURES "features"
#define DLP_YAML_FEATURES_METADATA "metadata"
#define DLP_YAML_FEATURES_CORE_AFFINITY "core_affinity"
#define DLP_YAML_FEATURES_IO "io"
#define DLP_YAML_FEATURES_IO_ENABLE "enable"
#define DLP_YAML_FEATURES_IO_POSIX "posix"
#define DLP_YAML_FEATURES_IO_STDIO "stdio"
#define DLP_YAML_FEATURES_TID "tid"
// INTERNAL
#define DLP_YAML_INTERNAL "internal"
#define DLP_YAML_INTERNAL_SIGNALS "bind_signals"
#define DLP_YAML_INTERNAL_THROW_ERROR "throw_error"
#define DLP_YAML_INTERNAL_WRITE_BUFFER_SIZE "write_buffer_size"


dlio_profiler::ConfigurationManager::ConfigurationManager()
    : enable(false), init_type(PROFILER_INIT_FUNCTION), log_file(),
      data_dirs(), trace_all_files(false), logger_level(cpplogger::LOG_ERROR),
      compression(false), gotcha_priority(1), metadata(false), core_affinity(false),
      io(true), stdio(true), posix(true), tids(true), bind_signals(false),
      throw_error(false),write_buffer_size(10) {
  const char *env_conf = getenv(DLIO_PROFILER_CONFIGURATION);
  YAML::Node config;
  if (env_conf != nullptr) {
    config = YAML::LoadFile(env_conf);
    if (config[DLP_YAML_PROFILER]) {
      if (config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_LOG_LEVEL]) {
        convert(config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_LOG_LEVEL]
                    .as<std::string>(),
                this->logger_level);
      }
    }
  }
  const char *env_log_level = getenv(DLIO_PROFILER_LOG_LEVEL);
  if (env_log_level != nullptr) {
    convert(env_log_level, this->logger_level);
  }
  DLIO_PROFILER_LOGGER->level = logger_level;
  DLIO_PROFILER_LOGDEBUG("Enabling logging level %d", logger_level);
  if (env_conf != nullptr) {
    this->enable = config[DLP_YAML_ENABLE].as<bool>();
    DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.enable %d",this->enable);
    if (config[DLP_YAML_PROFILER]) {
      if (config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_LOG_LEVEL]) {
        convert(config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_LOG_LEVEL]
                    .as<std::string>(),
                this->logger_level);
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.logger_level %d",this->logger_level);
      if (config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_INIT]) {
        convert(
            config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_INIT].as<std::string>(),
            this->init_type);
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.init_type %d",this->init_type);
      if (config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_LOG_FILE]) {
        this->log_file = config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_LOG_FILE]
                             .as<std::string>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.log_file %s",this->log_file.c_str());
      if (config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_DATA_DIRS]) {
        auto data_dirs_str =
            config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_DATA_DIRS]
                .as<std::string>();
        if (data_dirs_str == DLIO_PROFILER_ALL_FILES) {
          this->trace_all_files = true;
        } else {
          this->data_dirs = data_dirs_str;
        }
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.data_dirs_str %s",this->data_dirs.c_str());
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.trace_all_files %d",this->trace_all_files);

      if (config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_COMPRESSION]) {
        this->compression =
            config[DLP_YAML_PROFILER][DLP_YAML_PROFILER_COMPRESSION].as<bool>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.compression %d",this->compression);
    }
    if (config[DLP_YAML_GOTCHA]) {
      if (config[DLP_YAML_GOTCHA][DLP_YAML_GOTCHA_PRIORITY]) {
        this->gotcha_priority =
            config[DLP_YAML_GOTCHA][DLP_YAML_GOTCHA_PRIORITY].as<int>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.gotcha_priority %d",this->gotcha_priority);
    }
    if (config[DLP_YAML_FEATURES]) {
      if (config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_METADATA]) {
        this->metadata =
            config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_METADATA].as<bool>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.metadata %d",this->metadata);
      if (config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_CORE_AFFINITY]) {
        this->core_affinity =
            config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_CORE_AFFINITY]
                .as<bool>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.core_affinity %d",this->core_affinity);
      if (config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO] && config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO_ENABLE]) {
        this->io = config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO][DLP_YAML_FEATURES_IO_ENABLE].as<bool>();
        DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.io %d",this->io);
        if (this->io) {
          if (config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO]
                    [DLP_YAML_FEATURES_IO_POSIX]) {
            this->posix = config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO]
                                [DLP_YAML_FEATURES_IO_POSIX]
                                    .as<bool>();
          }
          DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.posix %d",this->posix);
          if (config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO]
                    [DLP_YAML_FEATURES_IO_STDIO]) {
            this->stdio = config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_IO]
                                [DLP_YAML_FEATURES_IO_STDIO]
                                    .as<bool>();
          }
          DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.stdio %d",this->stdio);
        }
      }
      if (config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_TID]) {
        this->tids =
            config[DLP_YAML_FEATURES][DLP_YAML_FEATURES_TID].as<bool>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.tids %d",this->tids);
    }
    if (config[DLP_YAML_INTERNAL]) {
      if (config[DLP_YAML_INTERNAL][DLP_YAML_INTERNAL_SIGNALS]) {
        this->bind_signals =
            config[DLP_YAML_INTERNAL][DLP_YAML_INTERNAL_SIGNALS].as<bool>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.bind_signals %d",this->bind_signals);
      if (config[DLP_YAML_INTERNAL][DLP_YAML_INTERNAL_THROW_ERROR]) {
        this->throw_error =
            config[DLP_YAML_INTERNAL][DLP_YAML_INTERNAL_THROW_ERROR].as<bool>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.throw_error %d",this->throw_error);
      if (config[DLP_YAML_INTERNAL][DLP_YAML_INTERNAL_WRITE_BUFFER_SIZE]) {
        this->write_buffer_size =
            config[DLP_YAML_INTERNAL][DLP_YAML_INTERNAL_WRITE_BUFFER_SIZE].as<size_t>();
      }
      DLIO_PROFILER_LOGDEBUG("YAML ConfigurationManager.write_buffer_size %d",this->write_buffer_size);
    }
  }
  const char *env_enable = getenv(DLIO_PROFILER_ENABLE);
  if (env_enable != nullptr && strcmp(env_enable, "1") == 0) {
    this->enable = true;
  }
  DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.enable %d",this->enable);
  if (this->enable) {
    const char *env_init_type = getenv(DLIO_PROFILER_INIT);
    if (env_init_type != nullptr) {
      convert(env_init_type, this->init_type);
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.init_type %d",this->init_type);
    const char *env_bind_signals = getenv(DLIO_PROFILER_BIND_SIGNALS);
    if (env_bind_signals != nullptr && strcmp(env_bind_signals, "1") == 0) {
      bind_signals = true;
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.bind_signals %d",this->bind_signals);
    const char *env_meta = getenv(DLIO_PROFILER_INC_METADATA);
    if (env_meta != nullptr && strcmp(env_meta, "1") == 0) {
      metadata = true;
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.metadata %d",this->metadata);

    const char *env_gotcha_priority = getenv(DLIO_PROFILER_GOTCHA_PRIORITY);
    if (env_gotcha_priority != nullptr) {
      this->gotcha_priority = atoi(env_gotcha_priority);  // GCOV_EXCL_LINE
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.gotcha_priority %d",this->gotcha_priority);
    const char *env_log_file = getenv(DLIO_PROFILER_LOG_FILE);
    if (env_log_file != nullptr) {
      this->log_file = env_log_file;
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.log_file %s",this->log_file.c_str());
    const char *env_data_dirs = getenv(DLIO_PROFILER_DATA_DIR);
    if (env_data_dirs != nullptr) {
      if (strcmp(env_data_dirs, DLIO_PROFILER_ALL_FILES) == 0) {
        this->trace_all_files = true;
      } else {
        this->data_dirs = env_data_dirs;
      }
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.data_dirs %s",this->data_dirs.c_str());
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.trace_all_files %d",this->trace_all_files);
    const char *disable_io = getenv(DLIO_PROFILER_DISABLE_IO);
    if (disable_io != nullptr && strcmp(disable_io, "1") == 0) {
      this->io = false;
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.io %d",this->io);
    if (this->io) {
      const char *disable_posix = getenv(DLIO_PROFILER_DISABLE_POSIX);
      if (disable_posix != nullptr && strcmp(disable_posix, "1") == 0) {
        this->posix = false;
      }
      DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.posix %d",this->posix);
      const char *disable_stdio = getenv(DLIO_PROFILER_DISABLE_STDIO);
      if (disable_stdio != nullptr && strcmp(disable_stdio, "1") == 0) {
        this->stdio = false;
      }
      DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.stdio %d",this->stdio);
    }
    const char *env_tid = getenv(DLIO_PROFILER_DISABLE_TIDS);
    if (env_tid != nullptr && strcmp(env_tid, "0") == 0) {
      this->tids = false;
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.tids %d",this->tids);
    const char *env_throw_error = getenv(DLIO_PROFILER_ERROR);
    if (env_throw_error != nullptr && strcmp(env_throw_error, "1") == 0) {
      this->throw_error = true; // GCOVR_EXCL_LINE
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.throw_error %d",this->throw_error);
    const char *env_compression = getenv(DLIO_PROFILER_TRACE_COMPRESSION);
    if (env_compression != nullptr && strcmp(env_compression, "0") == 0) {
      this->compression = false;
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.compression %d",this->compression);
    const char *env_write_buf_size = getenv(DLIO_PROFILER_WRITE_BUFFER_SIZE);
    if (env_write_buf_size != nullptr) {
      this->write_buffer_size = atoi(env_write_buf_size);
    }
    DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager.write_buffer_size %d",this->write_buffer_size);
  }
  DLIO_PROFILER_LOGDEBUG("ENV ConfigurationManager finished","");
}
