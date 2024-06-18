#include <dlio_profiler/dlio_logger.h>
template <>
std::shared_ptr<DLIOLogger> dlio_profiler::Singleton<DLIOLogger>::instance =
    nullptr;
template <>
bool dlio_profiler::Singleton<DLIOLogger>::stop_creating_instances = false;