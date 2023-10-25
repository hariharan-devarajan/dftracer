//
// Created by haridev on 10/25/23.
//
#include <dlio_profiler/dlio_logger.h>
#include <uv.h>

typedef std::unordered_map<std::string, std::any> MetadataType;
struct EventData {
  std::string event_name;
  std::string category;
  TimeResolution start_time;
  TimeResolution duration;
  MetadataType metadata;
  bool dlio_profiler_tid;
  ProcessID process_id;
  EventData(ConstEventType event_name, ConstEventType category,
            TimeResolution start_time, TimeResolution duration,
            MetadataType* metadata, bool dlio_profiler_tid, ProcessID process_id):event_name(event_name), category(category),
                                                                                  start_time(start_time), duration(duration),
                                                                                  metadata(), dlio_profiler_tid(dlio_profiler_tid), process_id(process_id){
    if (metadata != nullptr) this->metadata = *metadata;
  }
};

inline void log_event_uv(uv_work_t *req) {
  EventData* event = (EventData *) req->data;
  DLIO_PROFILER_LOGDEBUG("log_event_uv","");
  ThreadID tid = 0;
  if (event->dlio_profiler_tid) {
    tid = dlp_gettid() + event->process_id;
  }
  auto writer = dlio_profiler::Singleton<dlio_profiler::ChromeWriter>::get_instance(-1);
  if (writer != nullptr) {
    writer->log(event->event_name.c_str(), event->category.c_str(), event->start_time, event->duration, &event->metadata, event->process_id, tid);
  }
}
inline void log_event_uv_complete(uv_work_t *req, int status) {
  EventData* event = (EventData *) req->data;
  if (event != nullptr) {
    if (status == 0) {
      DLIO_PROFILER_LOGDEBUG(
          "log_event_uv was sucessful for event %s and cat %s",
          event->event_name.c_str(), event->category.c_str());
    } else {
      DLIO_PROFILER_LOGERROR(
          "log_event_uv was unsucessful for event %s and cat %s",
          event->event_name.c_str(), event->category.c_str());
    }
    delete (event);
  }
  free(req);
}
void DLIOLogger::log(ConstEventType event_name, ConstEventType category,
                     TimeResolution start_time, TimeResolution duration,
                     std::unordered_map<std::string, std::any> *metadata) {
  DLIO_PROFILER_LOGDEBUG("DLIOLogger.log","");
  auto req = new uv_work_t();
  auto *e = new EventData(event_name, category, start_time, duration, metadata, dlio_profiler_tid, process_id);
  req->data = e;
  uv_queue_work(uv_default_loop(), req, log_event_uv, log_event_uv_complete);
}

void DLIOLogger::finalize(){
  DLIO_PROFILER_LOGDEBUG("DLIOLogger.finalize","");
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  auto writer = dlio_profiler::Singleton<dlio_profiler::ChromeWriter>::get_instance(-1);
  if (writer != nullptr) {
    writer->finalize();
    dlio_profiler::Singleton<dlio_profiler::ChromeWriter>::finalize();
    DLIO_PROFILER_LOGINFO("Released Logger","");
  }
}