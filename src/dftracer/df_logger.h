//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_GENERIC_LOGGER_H
#define DFTRACER_GENERIC_LOGGER_H

#include <dftracer/core/logging.h>
#include <dftracer/core/singleton.h>
#include <dftracer/utils/configuration_manager.h>
#include <dftracer/utils/md5.h>
#include <dftracer/utils/utils.h>
#include <dftracer/writer/chrome_writer.h>
#include <libgen.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <any>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <dftracer/dftracer_config.hpp>
#include <unordered_map>

#ifdef DFTRACER_MPI_ENABLE
#include <mpi.h>
#endif

typedef std::chrono::high_resolution_clock chrono;

class DFTLogger {
 private:
  bool throw_error;
  bool is_init, dftracer_tid;
  ProcessID process_id;
  std::shared_ptr<dftracer::ChromeWriter> writer;
  uint32_t level;
  std::vector<int> index_stack;
  std::unordered_map<std::string, uint16_t> computed_hash;
  std::atomic_int index;
  bool has_entry;
#ifdef DFTRACER_MPI_ENABLE
  bool mpi_event;
#endif
 public:
  bool include_metadata;
  DFTLogger(bool init_log = false)
      : is_init(false),
        dftracer_tid(false),
        level(0),
        index_stack(),
        computed_hash(),
        index(0),
        has_entry(false),
#ifdef DFTRACER_MPI_ENABLE
        mpi_event(false),
#endif
        include_metadata(false) {
    DFTRACER_LOG_DEBUG("DFTLogger.DFTLogger", "");
    auto conf =
        dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
    include_metadata = conf->metadata;
    dftracer_tid = conf->tids;
    throw_error = conf->throw_error;
    this->is_init = true;
  }
  ~DFTLogger() { index_stack.clear(); }
  inline void update_log_file(std::string log_file, std::string exec_name,
                              std::string cmd, ProcessID process_id = -1) {
    DFTRACER_LOG_DEBUG("DFTLogger.update_log_file %s", log_file.c_str());
    this->process_id = process_id;
    this->writer = dftracer::Singleton<dftracer::ChromeWriter>::get_instance();
    if (this->writer != nullptr) {
      this->writer->initialize(log_file.data(), this->throw_error);
      std::unordered_map<std::string, std::any> *meta = nullptr;
      if (include_metadata) {
        meta = new std::unordered_map<std::string, std::any>();
        meta->insert_or_assign("version", DFTRACER_VERSION);
        meta->insert_or_assign("exec", exec_name);
        meta->insert_or_assign("cmd", cmd);
        time_t ltime;       /* calendar time */
        ltime = time(NULL); /* get current cal time */
        char timestamp[1024];
        auto size = sprintf(timestamp, "%s", asctime(localtime(&ltime)));
        timestamp[size - 1] = '\0';
        meta->insert_or_assign("date", std::string(timestamp));
        meta->insert_or_assign("ppid", getppid());
      }
      this->enter_event();
      this->log("start", "dftracer", this->get_time(), 0, meta);
      if (include_metadata) {
        delete (meta);
      }
      this->exit_event();
    }
    this->is_init = true;
    DFTRACER_LOG_INFO("Writing trace to %s", log_file.c_str());
  }

  inline void enter_event() {
    index++;
    level++;
    index_stack.push_back(index.load());
  }

  inline void exit_event() {
    level--;
    index_stack.pop_back();
  }

  inline TimeResolution get_time() {
    DFTRACER_LOG_DEBUG("DFTLogger.get_time", "");
    struct timeval tv {};
    gettimeofday(&tv, NULL);
    TimeResolution t = 1000000 * tv.tv_sec + tv.tv_usec;
    return t;
  }

  inline void log(ConstEventNameType event_name, ConstEventNameType category,
                  TimeResolution start_time, TimeResolution duration,
                  std::unordered_map<std::string, std::any> *metadata) {
    DFTRACER_LOG_DEBUG("DFTLogger.log", "");
    ThreadID tid = 0;
    if (dftracer_tid) {
      tid = df_gettid() + this->process_id;
    }
    int local_index;
    if (!include_metadata) {
      local_index = index.load();
    }
    if (metadata != nullptr) {
      metadata->insert_or_assign("level", level);
      int parent_index_value = -1;
      if (level > 1) {
        parent_index_value = index_stack[level - 2];
      }
      metadata->insert_or_assign("p_idx", parent_index_value);
    }
#ifdef DFTRACER_MPI_ENABLE
    if (!mpi_event && include_metadata) {
      int initialized;
      int status = MPI_Initialized(&initialized);
      if (status == MPI_SUCCESS && initialized == true &&
          this->writer != nullptr) {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        auto meta = std::unordered_map<std::string, std::any>();
        meta.insert_or_assign("rank", rank);
        auto start = this->get_time();
        this->enter_event();
        this->writer->log(index_stack[level - 1], "mpi", "dftracer",
                          EventType::COMPLETE_EVENT, start, 0, &meta,
                          this->process_id, tid);
        this->exit_event();
        char process_name[1024];
        auto size = sprintf(process_name, "Rank %d", rank);
        process_name[size] = '\0';
        this->enter_event();
        this->writer->log(index_stack[level - 1], "process_name", process_name,
                          EventType::METADATA_EVENT, 0, 0, nullptr,
                          this->process_id, tid);
        this->exit_event();

        mpi_event = true;
      }
    }
#endif

    if (this->writer != nullptr) {
      if (include_metadata) {
        this->writer->log(index_stack[level - 1], event_name, category,
                          EventType::COMPLETE_EVENT, start_time, duration,
                          metadata, this->process_id, tid);
      } else {
        this->writer->log(local_index, event_name, category,
                          EventType::COMPLETE_EVENT, start_time, duration,
                          metadata, this->process_id, tid);
      }

      has_entry = true;
    } else {
      DFTRACER_LOG_ERROR("DFTLogger.log writer not initialized", "");
    }
  }

  inline uint16_t hash_and_store(char *filename) {
    if (filename == NULL) return 0;
    auto iter = computed_hash.find(filename);
    uint16_t hash;
    if (iter == computed_hash.end()) {
      md5String(filename, &hash);
      computed_hash.insert_or_assign(filename, hash);
      if (this->writer != nullptr) {
        ThreadID tid = 0;
        if (dftracer_tid) {
          tid = df_gettid();
        }
        this->enter_event();
        this->writer->log(index_stack[level - 1], filename,
                          std::to_string(hash).c_str(), EventType::HASH_EVENT,
                          0, 0, nullptr, this->process_id, tid);
        this->exit_event();
      }
    } else {
      hash = iter->second;
    }
    return hash;
  }

  inline uint16_t hash_and_store(const char *filename) {
    if (filename == NULL) return 0;
    auto file = std::string(filename);
    return hash_and_store(file.data());
  }

  inline void finalize() {
    DFTRACER_LOG_DEBUG("DFTLogger.finalize", "");
    if (this->writer != nullptr) {
      auto meta = std::unordered_map<std::string, std::any>();
      meta.insert_or_assign("num_events", index.load());
      this->enter_event();
      this->log("end", "dftracer", this->get_time(), 0, &meta);
      this->exit_event();
      writer->finalize(has_entry);
      DFTRACER_LOG_INFO("Released Logger", "");
      this->writer = nullptr;
    } else {
      DFTRACER_LOG_WARN("DFTLogger.finalize writer not initialized", "");
    }
  }
};

#define DFT_LOGGER_INIT() dftracer::Singleton<DFTLogger>::get_instance()
#define DFT_LOGGER_FINI() \
  dftracer::Singleton<DFTLogger>::get_instance()->finalize()
#define DFT_LOGGER_UPDATE(value)               \
  if (trace && this->logger->include_metadata) \
    metadata->insert_or_assign(#value, value);
#define DFT_LOGGER_START(entity)                                  \
  DFTRACER_LOG_DEBUG("Calling function %s", __FUNCTION__);        \
  uint16_t fhash = is_traced(entity, __FUNCTION__);               \
  bool trace = fhash != 0;                                        \
  TimeResolution start_time = 0;                                  \
  std::unordered_map<std::string, std::any> *metadata = nullptr;  \
  if (trace) {                                                    \
    if (this->logger->include_metadata) {                         \
      metadata = new std::unordered_map<std::string, std::any>(); \
      DFT_LOGGER_UPDATE(fhash);                                   \
    }                                                             \
    this->logger->enter_event();                                  \
    start_time = this->logger->get_time();                        \
  }
#define DFT_LOGGER_START_ALWAYS()                                 \
  DFTRACER_LOG_DEBUG("Calling function %s", __FUNCTION__);        \
  bool trace = true;                                              \
  TimeResolution start_time = 0;                                  \
  std::unordered_map<std::string, std::any> *metadata = nullptr;  \
  if (trace) {                                                    \
    if (this->logger->include_metadata) {                         \
      metadata = new std::unordered_map<std::string, std::any>(); \
    }                                                             \
    this->logger->enter_event();                                  \
    start_time = this->logger->get_time();                        \
  }
#define DFT_LOGGER_END()                                          \
  if (trace) {                                                    \
    TimeResolution end_time = this->logger->get_time();           \
    this->logger->log((char *)__FUNCTION__, CATEGORY, start_time, \
                      end_time - start_time, metadata);           \
    this->logger->exit_event();                                   \
    if (this->logger->include_metadata) delete (metadata);        \
  }

#endif  // DFTRACER_GENERIC_LOGGER_H
