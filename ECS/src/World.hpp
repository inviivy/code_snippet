#pragma once

#include "ComponentTypeInfo.hpp"
#include "ResourceTypeInfo.hpp"
#include "util.hpp"

#include <cassert>
#include <concepts>
#include <cstdint>
#include <functional>
#include <new>
#include <optional>
#include <type_traits>
#include <unordered_map>

namespace ecs {
using ComponentTypeID = uint32_t;
using Entity = uint32_t;

struct Commands;
struct Resource;
struct Queryer;
struct Events;

template <typename T> struct EventStaging final {
  static bool Has() { return data_.has_value(); }

  static void Set(const T &t) { data_ = t; }

  static T &Get() { return data_.value(); }

  static void Clear() { data_ = std::nullopt; }

private:
  inline static std::optional<T> data_ = std::nullopt;
};

// Event的作用是在各个system中传递数据
// n Reader : 1 EventStaging<T>
template <typename T> struct EventReader final {
  bool Has() { return EventStaging<T>::Has(); }
  T Read() { return EventStaging<T>::Get(); }
};

// n writter : 1 EventStaging<T>
// 也就是说, 各个system在每一帧只能有一个T类型的write,
// 如果需要在一帧中传递多个event 则需要使用不同的Type传递, 否则数据直接覆盖,
// 只有最后调用的哪个write生效. 实现上是: 当前帧(loop)write
// data，下一帧其他system read data 如果其它系统忽略了数据,
// 那么下下帧这个数据会被清除
template <typename T> struct EventWriter {
  EventWriter(Events &event) : events_(event) {}

  void Write(T &&);

private:
  Events &events_;
};

struct Events final {

  friend struct World;

  template <typename T> friend struct EventWriter;

  template <typename T> auto Reader() { return EventReader<T>{}; }
  template <typename T> auto Writer() { return EventWriter<T>{*this}; }

private:
  void addAllEvents() {
    for (auto &func : addEventFuncs_) {
      func();
    }
    addEventFuncs_.clear();
  }

  void removeOldEvents() {
    for (auto &func : removeOldEventFuncs_) {
      func();
    }
    removeOldEventFuncs_.swap(removeEventFuncs_);
    removeEventFuncs_.clear();
  }

private:
  /* 这里的设计思路是: 当我们添加一个事件时, 我们需要额外添加事件结束后的动作,
   * 所以增加了两个std::vector<void(*)()> */
  std::vector<void (*)()> removeEventFuncs_;
  // 每一帧执行时需要将上一帧的事件全部清除掉, 即执行析构等相关操作
  // 本质上就是解决 当前帧要析构上一帧的事件 的问题
  std::vector<void (*)()> removeOldEventFuncs_;
  // 将要添加的事件抽象为一个带状态的function, 然后执行function才真正添加成功,
  // 每一帧执行一次
  std::vector<std::function<void()>> addEventFuncs_;
};

template <typename T> void EventWriter<T>::Write(T &&t) {
  // 其实就是添加了一个创建对象的functor
  events_.addEventFuncs_.push_back(
      [t = std::forward<T>(t)]() { EventStaging<T>::Set(t); });

  // 添加一个删除(之前创建的对象)的functor
  events_.removeEventFuncs_.push_back([]() { EventStaging<T>::Clear(); });
};

using UpdateSystem = void (*)(Commands, Queryer, Resource, Events &);
using StartupSystem = void (*)(Commands);

struct World {
  friend class Commands;
  friend class Resource;
  friend class Queryer;

  using ComponentMap = std::unordered_map<ComponentTypeID, ComponentTypeInfo>;
  using ComponentContainer = std::unordered_map<ComponentTypeID, void *>;

  World() {}

  World(const World &) = delete;
  World &operator=(const World &) = delete;

  ~World() { Shutdown(); }

  World &AddStartupSystem(StartupSystem startupSystem) {
    startupSystems_.push_back(startupSystem);
    return *this;
  }

  World &AddSystem(UpdateSystem updateSystem) {
    updateSystems_.push_back(updateSystem);
    return *this;
  }

  template <typename T> World &SetResource(T &&resource);

  void Startup();
  void Update();
  // raii就行
  void Shutdown() {
    resources_.clear();

    for (auto &[_, componentContainer] : entities_) {
      for (auto &[componentTypeId, ptr] : componentContainer) {
        assert(ptr != nullptr);
        componentMap_[componentTypeId].destroyFn_(ptr);
      }
    }
    entities_.clear();

    componentMap_.clear();
  }

private:
  // for category
  struct CategoryResource {};
  struct CategoryComponent {};

private:
  ComponentMap componentMap_;
  std::unordered_map<Entity, ComponentContainer> entities_;
  /* 一个类型的resource只允许存在一个, 比如Timer，Render等.
   * ResourceTypeInfo支持raii */
  std::unordered_map<ComponentTypeID, ResourceTypeInfo> resources_;
  std::vector<StartupSystem> startupSystems_;
  std::vector<UpdateSystem> updateSystems_;
  Events events_;
};

struct Commands final {
  explicit Commands(World &world) : world_(world) {}

  template <typename... ComponentTypes>
  Commands &Spawn(ComponentTypes &&...components) {
    Entity entity = IdGenerator<uint32_t>::Gen();
    do_spawn(entity, std::forward<ComponentTypes>(components)...);
    return *this;
  }

  Commands &Destroy(Entity entity) {
    if (auto it = world_.entities_.find(entity); it != world_.entities_.end()) {
      for (auto &[componentTypeId, ptr] : it->second) {
        auto &componentInfo = world_.componentMap_[componentTypeId];
        componentInfo.destroyFn_(ptr);
        componentInfo.sparseSet_.Remove(entity);
      }
      world_.entities_.erase(it);
    }

    return *this;
  }

  template <typename T> Commands &SetResource(T &&resource) {
    auto resourceTypeIndex = TypeIndexGetter<World::CategoryResource>::Get<T>();
    if (auto it = world_.resources_.find(resourceTypeIndex);
        it == world_.resources_.end()) {
      world_.resources_.emplace(resourceTypeIndex,
                                ResourceTypeInfo(
                                    []() -> void * {
                                      return ::operator new(
                                          sizeof(std::decay_t<T>));
                                    },
                                    [](void *elem) {
                                      using Type = std::decay_t<T>;
                                      static_cast<Type *>(elem)->~Type();
                                      ::operator delete(elem);
                                    }));
    }

    ResourceTypeInfo &resourceTypeInfo = world_.resources_[resourceTypeIndex];
    // 暂时不考虑内存分配失败的情况
    resourceTypeInfo.source_ = resourceTypeInfo.createFn_();
    ::new (resourceTypeInfo.source_) std::decay_t<T>(std::forward<T>(resource));
    return *this;
  }

  template <typename T> Commands &RemoveResource() {
    auto resourceTypeIndex = TypeIndexGetter<World::CategoryResource>::Get<T>();
    world_.resources_.erase(resourceTypeIndex);
    return *this;
  }

private:
  template <typename T, typename... Remains>
  void do_spawn(Entity entity, T &&component, Remains &&...remains) {
    // 类型index

    auto componentTypeIndex =
        TypeIndexGetter<World::CategoryComponent>::Get<T>();
    if (auto it = world_.componentMap_.find(componentTypeIndex);
        it == world_.componentMap_.end()) {
      // 插入Component的构造和析构函数
      world_.componentMap_.emplace(componentTypeIndex,
                                   ComponentTypeInfo(
                                       []() -> void * {
                                         return ::operator new(
                                             sizeof(std::decay_t<T>));
                                       },
                                       [](void *elem) {
                                         using Type = std::decay_t<T>;
                                         static_cast<Type *>(elem)->~Type();
                                         ::operator delete(elem);
                                       }));
    }
    ComponentTypeInfo &componentTypeInfo =
        world_.componentMap_[componentTypeIndex];
    // create component object and initial
    void *element = componentTypeInfo.createFn_();
    ::new (element) std::decay_t<T>(std::forward<T>(component));
    // 记录拥有此类型component的entity，便于反向查找entity
    componentTypeInfo.sparseSet_.Add(entity);

    // 检查entity是否存在
    if (auto it = world_.entities_.find(entity); it == world_.entities_.end()) {
      auto [componentIt, succ] =
          world_.entities_.emplace(entity, World::ComponentContainer{});
      componentIt->second.emplace(componentTypeIndex, element);
    } else {
      it->second.emplace(componentTypeIndex, element);
    }

    if constexpr (sizeof...(remains) != 0) {
      do_spawn(entity, std::forward<Remains>(remains)...);
    }
  }

private:
  World &world_;
};

struct Resource final {
  Resource(World &world) : world_(world) {}

  template <typename T>
  requires(!std::is_reference_v<T> && !std::is_pointer_v<T>) bool
  Has() const {
    auto resourceTypeIndex = TypeIndexGetter<World::CategoryResource>::Get<T>();
    return world_.resources_.find(resourceTypeIndex) != world_.resources_.end();
  }

  template <typename T>
  requires(!std::is_reference_v<T> && !std::is_pointer_v<T>)
  T &Get() {
    auto resourceTypeIndex = TypeIndexGetter<World::CategoryResource>::Get<T>();
    // non error handle
    return *static_cast<T *>(world_.resources_[resourceTypeIndex].source_);
  }

private:
  World &world_;
};

struct Queryer final {
  Queryer(World &world) : world_(world) {}

  template <typename... Components>
  requires((!std::is_reference_v<Components> &&
            !std::is_pointer_v<Components>) &&
           ...)
  std::vector<Entity> Query() {
    std::vector<Entity> entities;
    do_query<Components...>(entities);
    return entities;
  }

  template <typename T>
  requires(!std::is_reference_v<T> && !std::is_pointer_v<T>) bool
  Has(Entity entity) {
    auto it = world_.entities_.find(entity);
    auto componentTypeIndex =
        TypeIndexGetter<World::CategoryComponent>::Get<T>();
    return it != world_.entities_.end() &&
           it->second.find(componentTypeIndex) != it->second.end();
  }

  // 先Has再Get
  template <typename T>
  requires(!std::is_reference_v<T> && !std::is_pointer_v<T>)
  T &Get(Entity entity) {
    auto componentTypeIndex =
        TypeIndexGetter<World::CategoryComponent>::Get<T>();
    return *static_cast<T *>(world_.entities_[entity][componentTypeIndex]);
  }

private:
  /* 查找拥有T或同时拥有T1/T2/T3...类型的entity */
  template <typename T, typename... Remains>
  void do_query(std::vector<Entity> &outEntities) {
    auto componentTypeIndex =
        TypeIndexGetter<World::CategoryComponent>::Get<T>();
    // componentMap可能不存在对应的type info
    if (auto it = world_.componentMap_.find(componentTypeIndex);
        it != world_.componentMap_.end()) {
      for (auto entity : it->second.sparseSet_) {
        if constexpr (sizeof...(Remains) != 0) {
          // 说明需要查询T1/T2/T3...这样的组合是否存在, 检查每一个entity
          if (isSame<Remains...>(entity, outEntities)) {
            outEntities.push_back(entity);
          }
        } else {
          // 只有在匹配到最后一个时才将entity压入结果集
          outEntities.push_back(entity);
        }
      }
    }
  }

  template <typename T, typename... Remains>
  bool isSame(Entity entity, std::vector<Entity> &outEntities) const {
    auto componentTypeIndex =
        TypeIndexGetter<World::CategoryComponent>::Get<T>();
    auto &componentContainer = world_.entities_[entity];
    if (auto it = componentContainer.find(componentTypeIndex);
        it == componentContainer.end()) {
      return false;
    }
    // 说明该entity拥有T1/T2/T3...这些component
    if constexpr (sizeof...(Remains) != 0) {
      return isSame<Remains...>(entity, outEntities);
    } else {
      return true;
    }
  }

private:
  World &world_;
};

template <typename T> World &World::SetResource(T &&resource) {
  Commands command(*this);
  command.SetResource(std::forward<T>(resource));
  return *this;
}

inline void World::Startup() {
  for (auto &sys : startupSystems_) {
    sys(Commands{*this});
  }
}

inline void World::Update() {
  for (auto &sys : updateSystems_) {
    sys(Commands{*this}, Queryer{*this}, Resource{*this}, events_);
  }
  events_.removeOldEvents();
  events_.addAllEvents();
}

}; // namespace ecs