#include <dftracer/utils/utils.h>
template <>
std::shared_ptr<Trie> dftracer::Singleton<Trie>::instance = nullptr;
template <>
bool dftracer::Singleton<Trie>::stop_creating_instances = false;