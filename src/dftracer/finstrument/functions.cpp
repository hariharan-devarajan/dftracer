#include <dftracer/finstrument/functions.h>
#include <link.h>
std::shared_ptr<dftracer::Function> dftracer::Function::instance = nullptr;

thread_local std::unordered_map<ConstEventNameType, TimeResolution>
    dftracer::Function::map =
        std::unordered_map<ConstEventNameType, TimeResolution>();
bool dftracer::Function::stop_trace = false;
int dftracer::Function::enter_event(ConstEventNameType name) {
  if (stop_trace) return -1;
  auto start = this->logger->get_time();
  map.insert_or_assign(name, start);
  return 0;
}

int dftracer::Function::exit_event(ConstEventNameType name,
                                   TimeResolution &start) {
  if (stop_trace) return -1;
  auto tmap = map.find(name);
  if (tmap != map.end()) {
    start = tmap->second;
    map.erase(tmap);
    return 0;
  }
  return -1;
}

void __cyg_profile_func_enter(void *func, void *caller) {
  auto function = dftracer::Function::get_instance();
  Dl_info info;
  if (!dladdr(func, &info)) return;
  if (!info.dli_sname || !info.dli_fname) return;
  if (!function->is_active()) return;
  ConstEventNameType event_name = info.dli_sname;
  DFTRACER_LOG_DEBUG("Calling function %s", event_name);
  function->enter_event(event_name);
}

void __cyg_profile_func_exit(void *func, void *caller) {
  Dl_info info;
  if (!dladdr(func, &info)) return;
  if (!info.dli_sname || !info.dli_fname) return;
  auto function = dftracer::Function::get_instance();
  if (!function->is_active()) return;
  auto end_time = function->logger->get_time();
  ConstEventNameType event_name = info.dli_sname;
  TimeResolution start_time;
  int status = function->exit_event(event_name, start_time);
  if (status == 0) {
    std::unordered_map<std::string, std::any> *metadata;
    if (function->logger->include_metadata) {
      metadata = new std::unordered_map<std::string, std::any>();
      const char *so = info.dli_fname;
      metadata->insert_or_assign("so", so);
    }
    function->logger->enter_event();
    function->logger->log(event_name, CATEGORY, start_time,
                          end_time - start_time, metadata);
    function->logger->exit_event();
  }
}