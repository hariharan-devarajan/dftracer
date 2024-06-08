#include <dlio_profiler/utils/utils.h>
template <>
std::shared_ptr<Trie> dlio_profiler::Singleton<Trie>::instance = nullptr;
template <>
bool dlio_profiler::Singleton<Trie>::stop_creating_instances = false;