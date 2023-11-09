//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_SINGLETON_H
#define DLIO_PROFILER_SINGLETON_H

#include <iostream>
#include <memory>
#include <utility>
/**
 * Make a class singleton when used with the class. format for class name T
 * Singleton<T>::GetInstance()
 * @tparam T
 */
namespace dlio_profiler {
    template<typename T>
    class Singleton {
    public:
        static bool stop_creating_instances;
        /**
         * Members of Singleton Class
         */
        /**
         * Uses unique pointer to build a static global instance of variable.
         * @tparam T
         * @return instance of T
         */
        template<typename... Args>
        static std::shared_ptr<T> get_instance(Args... args) {
          if (stop_creating_instances) return nullptr;
          if (instance == nullptr)
            instance = std::make_shared<T>(std::forward<Args>(args)...);
          return instance;
        }

        /**
         * Operators
         */
        Singleton &operator=(const Singleton) = delete; /* deleting = operatos*/
        /**
         * Constructor
         */
    public:
        Singleton(const Singleton &) = delete; /* deleting copy constructor. */
        static void finalize() {
          stop_creating_instances = true;
        }
    protected:
        static std::shared_ptr<T> instance;

        Singleton() {} /* hidden default constructor. */
    };

    template<typename T>
    std::shared_ptr<T> Singleton<T>::instance = nullptr;
    template<typename T>
    bool Singleton<T>::stop_creating_instances = false;
}  // namespace dlio_profiler
#endif //DLIO_PROFILER_SINGLETON_H
