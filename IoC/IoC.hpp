#pragma once
#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

struct IoContainer {
  IoContainer() {}
  ~IoContainer() {}
  IoContainer(const IoContainer &) = delete;
  IoContainer &operator=(const IoContainer &) = delete;

  /* 相当于工厂模式, 根据输入strKey, T映射Depend */
  template <typename T, typename Depend, typename... Args>
  void RegisterType(const std::string &strKey) {
    // 闭包擦除了参数类型
    std::function<T *(Args...)> function = [](Args... args) {
      return new T{new Depend(args...)};
    };
    RegisterType(strKey, function);
  }

  template <typename T, typename... Args>
  T *Resolve(const std::string &strKey, Args... args) {
    if (m_creatorMap.find(strKey) == m_creatorMap.end()) {
      return nullptr;
    }

    std::any resolver = m_creatorMap[strKey];
    auto func = std::any_cast<std::function<T *(Args...)>>(resolver);
    return func(args...);
  }

  template <typename T, typename... Args>
  std::shared_ptr<T> ResolveShared(const std::string &strKey, Args... args) {
    T *t = Resolve<T>(strKey, args...);
    return std::shared_ptr<T>(t);
  }

private:
  void RegisterType(const std::string &strKey, std::any constructor) {
    if (m_creatorMap.find(strKey) != m_creatorMap.end()) {
      // 该类型已存在构造function
      throw std::invalid_argument("this key has already exist");
    }
    m_creatorMap.emplace(strKey, constructor);
  }

private:
  std::unordered_map<std::string, std::any> m_creatorMap;
};