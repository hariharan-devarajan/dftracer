#include <dftracer/df_logger.h>
template <>
std::shared_ptr<DFTLogger> dftracer::Singleton<DFTLogger>::instance = nullptr;
template <>
bool dftracer::Singleton<DFTLogger>::stop_creating_instances = false;