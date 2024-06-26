//
// Created by haridev on 10/27/23.
//

#include "configuration_manager.h"

#include <dftracer/core/constants.h>
#include <yaml-cpp/yaml.h>

#include "utils.h"

#define DFT_YAML_ENABLE "enable"
// PROFILER
#define DFT_YAML_PROFILER "profiler"
#define DFT_YAML_PROFILER_INIT "init"
#define DFT_YAML_PROFILER_LOG_FILE "log_file"
#define DFT_YAML_PROFILER_DATA_DIRS "data_dirs"
#define DFT_YAML_PROFILER_LOG_LEVEL "log_level"
#define DFT_YAML_PROFILER_COMPRESSION "compression"
// GOTCHA
#define DFT_YAML_GOTCHA "gotcha"
#define DFT_YAML_GOTCHA_PRIORITY "priority"
// Features
#define DFT_YAML_FEATURES "features"
#define DFT_YAML_FEATURES_METADATA "metadata"
#define DFT_YAML_FEATURES_CORE_AFFINITY "core_affinity"
#define DFT_YAML_FEATURES_IO "io"
#define DFT_YAML_FEATURES_IO_ENABLE "enable"
#define DFT_YAML_FEATURES_IO_POSIX "posix"
#define DFT_YAML_FEATURES_IO_STDIO "stdio"
#define DFT_YAML_FEATURES_TID "tid"
// INTERNAL
#define DFT_YAML_INTERNAL "internal"
#define DFT_YAML_INTERNAL_SIGNALS "bind_signals"
#define DFT_YAML_INTERNAL_THROW_ERROR "throw_error"
#define DFT_YAML_INTERNAL_WRITE_BUFFER_SIZE "write_buffer_size"
template <>
std::shared_ptr<dftracer::ConfigurationManager>
    dftracer::Singleton<dftracer::ConfigurationManager>::instance = nullptr;
template <>
bool dftracer::Singleton<
    dftracer::ConfigurationManager>::stop_creating_instances = false;
dftracer::ConfigurationManager::ConfigurationManager()
    : enable(false),
      init_type(PROFILER_INIT_FUNCTION),
      log_file(),
      data_dirs(),
      metadata(false),
      core_affinity(false),
      gotcha_priority(1),
      logger_level(cpplogger::LOG_ERROR),
      io(true),
      posix(true),
      stdio(true),
      compression(false),
      trace_all_files(false),
      tids(true),
      bind_signals(false),
      throw_error(false),
      write_buffer_size(10) {
  const char *env_conf = getenv(DFTRACER_CONFIGURATION);
  YAML::Node config;
  if (env_conf != nullptr) {
    config = YAML::LoadFile(env_conf);
    if (config[DFT_YAML_PROFILER]) {
      if (config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_LOG_LEVEL]) {
        convert(config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_LOG_LEVEL]
                    .as<std::string>(),
                this->logger_level);
      }
    }
  }
  const char *env_log_level = getenv(DFTRACER_LOG_LEVEL);
  if (env_log_level != nullptr) {
    convert(env_log_level, this->logger_level);
  }
  DFTRACER_LOGGER->level = logger_level;
  DFTRACER_LOGDEBUG("Enabling logging level %d", logger_level);
  if (env_conf != nullptr) {
    this->enable = config[DFT_YAML_ENABLE].as<bool>();
    DFTRACER_LOGDEBUG("YAML ConfigurationManager.enable %d", this->enable);
    if (config[DFT_YAML_PROFILER]) {
      if (config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_LOG_LEVEL]) {
        convert(config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_LOG_LEVEL]
                    .as<std::string>(),
                this->logger_level);
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.logger_level %d",
                        this->logger_level);
      if (config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_INIT]) {
        convert(
            config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_INIT].as<std::string>(),
            this->init_type);
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.init_type %d",
                        this->init_type);
      if (config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_LOG_FILE]) {
        this->log_file = config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_LOG_FILE]
                             .as<std::string>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.log_file %s",
                        this->log_file.c_str());
      if (config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_DATA_DIRS]) {
        auto data_dirs_str =
            config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_DATA_DIRS]
                .as<std::string>();
        if (data_dirs_str == DFTRACER_ALL_FILES) {
          this->trace_all_files = true;
        } else {
          this->data_dirs = data_dirs_str;
        }
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.data_dirs_str %s",
                        this->data_dirs.c_str());
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.trace_all_files %d",
                        this->trace_all_files);

      if (config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_COMPRESSION]) {
        this->compression =
            config[DFT_YAML_PROFILER][DFT_YAML_PROFILER_COMPRESSION].as<bool>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.compression %d",
                        this->compression);
    }
    if (config[DFT_YAML_GOTCHA]) {
      if (config[DFT_YAML_GOTCHA][DFT_YAML_GOTCHA_PRIORITY]) {
        this->gotcha_priority =
            config[DFT_YAML_GOTCHA][DFT_YAML_GOTCHA_PRIORITY].as<int>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.gotcha_priority %d",
                        this->gotcha_priority);
    }
    if (config[DFT_YAML_FEATURES]) {
      if (config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_METADATA]) {
        this->metadata =
            config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_METADATA].as<bool>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.metadata %d",
                        this->metadata);
      if (config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_CORE_AFFINITY]) {
        this->core_affinity =
            config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_CORE_AFFINITY]
                .as<bool>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.core_affinity %d",
                        this->core_affinity);
      if (config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO] &&
          config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO_ENABLE]) {
        this->io = config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO]
                         [DFT_YAML_FEATURES_IO_ENABLE]
                             .as<bool>();
        DFTRACER_LOGDEBUG("YAML ConfigurationManager.io %d", this->io);
        if (this->io) {
          if (config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO]
                    [DFT_YAML_FEATURES_IO_POSIX]) {
            this->posix = config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO]
                                [DFT_YAML_FEATURES_IO_POSIX]
                                    .as<bool>();
          }
          DFTRACER_LOGDEBUG("YAML ConfigurationManager.posix %d", this->posix);
          if (config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO]
                    [DFT_YAML_FEATURES_IO_STDIO]) {
            this->stdio = config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_IO]
                                [DFT_YAML_FEATURES_IO_STDIO]
                                    .as<bool>();
          }
          DFTRACER_LOGDEBUG("YAML ConfigurationManager.stdio %d", this->stdio);
        }
      }
      if (config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_TID]) {
        this->tids =
            config[DFT_YAML_FEATURES][DFT_YAML_FEATURES_TID].as<bool>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.tids %d", this->tids);
    }
    if (config[DFT_YAML_INTERNAL]) {
      if (config[DFT_YAML_INTERNAL][DFT_YAML_INTERNAL_SIGNALS]) {
        this->bind_signals =
            config[DFT_YAML_INTERNAL][DFT_YAML_INTERNAL_SIGNALS].as<bool>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.bind_signals %d",
                        this->bind_signals);
      if (config[DFT_YAML_INTERNAL][DFT_YAML_INTERNAL_THROW_ERROR]) {
        this->throw_error =
            config[DFT_YAML_INTERNAL][DFT_YAML_INTERNAL_THROW_ERROR].as<bool>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.throw_error %d",
                        this->throw_error);
      if (config[DFT_YAML_INTERNAL][DFT_YAML_INTERNAL_WRITE_BUFFER_SIZE]) {
        this->write_buffer_size =
            config[DFT_YAML_INTERNAL][DFT_YAML_INTERNAL_WRITE_BUFFER_SIZE]
                .as<size_t>();
      }
      DFTRACER_LOGDEBUG("YAML ConfigurationManager.write_buffer_size %d",
                        this->write_buffer_size);
    }
  }
  const char *env_enable = getenv(DFTRACER_ENABLE);
  if (env_enable != nullptr && strcmp(env_enable, "1") == 0) {
    this->enable = true;
  }
  DFTRACER_LOGDEBUG("ENV ConfigurationManager.enable %d", this->enable);
  if (this->enable) {
    const char *env_init_type = getenv(DFTRACER_INIT);
    if (env_init_type != nullptr) {
      convert(env_init_type, this->init_type);
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.init_type %d", this->init_type);
    const char *env_bind_signals = getenv(DFTRACER_BIND_SIGNALS);
    if (env_bind_signals != nullptr && strcmp(env_bind_signals, "1") == 0) {
      bind_signals = true;
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.bind_signals %d",
                      this->bind_signals);
    const char *env_meta = getenv(DFTRACER_INC_METADATA);
    if (env_meta != nullptr && strcmp(env_meta, "1") == 0) {
      metadata = true;
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.metadata %d", this->metadata);

    const char *env_gotcha_priority = getenv(DFTRACER_GOTCHA_PRIORITY);
    if (env_gotcha_priority != nullptr) {
      this->gotcha_priority = atoi(env_gotcha_priority);  // GCOV_EXCL_LINE
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.gotcha_priority %d",
                      this->gotcha_priority);
    const char *env_log_file = getenv(DFTRACER_LOG_FILE);
    if (env_log_file != nullptr) {
      this->log_file = env_log_file;
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.log_file %s",
                      this->log_file.c_str());
    const char *env_data_dirs = getenv(DFTRACER_DATA_DIR);
    if (env_data_dirs != nullptr) {
      if (strcmp(env_data_dirs, DFTRACER_ALL_FILES) == 0) {
        this->trace_all_files = true;
      } else {
        this->data_dirs = env_data_dirs;
      }
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.data_dirs %s",
                      this->data_dirs.c_str());
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.trace_all_files %d",
                      this->trace_all_files);
    const char *disable_io = getenv(DFTRACER_DISABLE_IO);
    if (disable_io != nullptr && strcmp(disable_io, "1") == 0) {
      this->io = false;
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.io %d", this->io);
    if (this->io) {
      const char *disable_posix = getenv(DFTRACER_DISABLE_POSIX);
      if (disable_posix != nullptr && strcmp(disable_posix, "1") == 0) {
        this->posix = false;
      }
      DFTRACER_LOGDEBUG("ENV ConfigurationManager.posix %d", this->posix);
      const char *disable_stdio = getenv(DFTRACER_DISABLE_STDIO);
      if (disable_stdio != nullptr && strcmp(disable_stdio, "1") == 0) {
        this->stdio = false;
      }
      DFTRACER_LOGDEBUG("ENV ConfigurationManager.stdio %d", this->stdio);
    }
    const char *env_tid = getenv(DFTRACER_DISABLE_TIDS);
    if (env_tid != nullptr && strcmp(env_tid, "0") == 0) {
      this->tids = false;
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.tids %d", this->tids);
    const char *env_throw_error = getenv(DFTRACER_ERROR);
    if (env_throw_error != nullptr && strcmp(env_throw_error, "1") == 0) {
      this->throw_error = true;  // GCOVR_EXCL_LINE
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.throw_error %d",
                      this->throw_error);
    const char *env_compression = getenv(DFTRACER_TRACE_COMPRESSION);
    if (env_compression != nullptr && strcmp(env_compression, "0") == 0) {
      this->compression = false;
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.compression %d",
                      this->compression);
    const char *env_write_buf_size = getenv(DFTRACER_WRITE_BUFFER_SIZE);
    if (env_write_buf_size != nullptr) {
      this->write_buffer_size = atoi(env_write_buf_size);
    }
    DFTRACER_LOGDEBUG("ENV ConfigurationManager.write_buffer_size %d",
                      this->write_buffer_size);
  }
  DFTRACER_LOGDEBUG("ENV ConfigurationManager finished", "");
}
