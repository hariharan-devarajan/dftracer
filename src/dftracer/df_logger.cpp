#include <dftracer/df_logger.h>
template <>
std::shared_ptr<DFTLogger> dftracer::Singleton<DFTLogger>::instance = nullptr;
template <>
bool dftracer::Singleton<DFTLogger>::stop_creating_instances = false;

thread_local uint32_t DFTLogger::level = 0;
thread_local std::vector<int> DFTLogger::index_stack = std::vector<int>();
thread_local std::unordered_map<std::string, uint16_t>
    DFTLogger::computed_hash = std::unordered_map<std::string, uint16_t>();