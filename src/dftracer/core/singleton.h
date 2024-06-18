//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_SINGLETON_H
#define DFTRACER_SINGLETON_H

#include <iostream>
#include <memory>
#include <utility>
/**
 * Make a class singleton when used with the class. format for class name T
 * Singleton<T>::GetInstance()
 * @tparam T
 */
namespace dftracer {
template <typename T>
class Singleton {
 public:
  /**
   * Members of Singleton Class
   */
  /**
   * Uses unique pointer to build a static global instance of variable.
   * @tparam T
   * @return instance of T
   */
  template <typename... Args>
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
 public:
  Singleton(const Singleton &) = delete; /* deleting copy constructor. */
  static void finalize() { stop_creating_instances = true; }

 protected:
  static bool stop_creating_instances;
  static std::shared_ptr<T> instance;

  Singleton() {} /* hidden default constructor. */
};

}  // namespace dftracer
#endif  // DFTRACER_SINGLETON_H
