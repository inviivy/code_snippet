#pragma once

#include <memory>
#include <utility>

template <typename T> struct Singleton {
  template <typename... Args>
  static std::shared_ptr<T> Instance(Args &&...args) {
    if (m_instance == nullptr) {
      // m_instance = new T(std::forward<Args>(args)...);
      m_instance = std::make_shared<T>(std::forward<Args>(args)...);
    }
    return m_instance;
  }

  static std::shared_ptr<T> GetInstance() {
    if (m_instance == nullptr) {
      // throw
    }
    return m_instance;
  }

  Singleton() = delete;
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

private:
  // static T *m_instance;
  static std::shared_ptr<T> m_instance;
};

template <typename T> std::shared_ptr<T> Singleton<T>::m_instance = nullptr;