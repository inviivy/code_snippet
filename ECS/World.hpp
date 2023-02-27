#pragma once

#include "ComponentTypeInfo.hpp"
#include "ResourceTypeInfo.hpp"
#include "util.hpp"

#include <cassert>
#include <concepts>
#include <cstdint>
#include <new>
#include <type_traits>
#include <unordered_map>

namespace ecs {
using ComponentTypeID = uint32_t;
using Entity = uint32_t;

struct Commands;
struct Resource;
struct Queryer;

using StartupSystem = void(*)(Commands, Queryer, Resource);

struct World {
  friend class Commands;
  friend class Resource;
  friend class Queryer;

  using ComponentMap = std::unordered_map<ComponentTypeID, ComponentTypeInfo>;
  using ComponentContainer = std::unordered_map<ComponentTypeID, void *>;

  World() {}

  World(const World &) = delete;
  World &operator=(const World &) = delete;

  ~World() {
    for (auto &[_, componentContainer] : entities_) {
      for (auto &[componentTypeId, ptr] : componentContainer) {
        assert(ptr != nullptr);
        componentMap_[componentTypeId].destroyFn_(ptr);
      }
    }
    entities_.clear();
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

    auto componentTypeIndex = TypeIndexGetter<World::CategoryComponent>::Get<T>();
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
    auto componentTypeIndex = TypeIndexGetter<World::CategoryComponent>::Get<T>();
    return it != world_.entities_.end() &&
           it->second.find(componentTypeIndex) != it->second.end();
  }

  // 先Has再Get
  template <typename T>
  requires(!std::is_reference_v<T> && !std::is_pointer_v<T>)
  T &Get(Entity entity) {
    auto componentTypeIndex = TypeIndexGetter<World::CategoryComponent>::Get<T>();
    return *static_cast<T *>(world_.entities_[entity][componentTypeIndex]);
  }

private:
  /* 查找拥有T或同时拥有T1/T2/T3...类型的entity */
  template <typename T, typename... Remains>
  void do_query(std::vector<Entity> &outEntities) {
    auto componentTypeIndex = TypeIndexGetter<World::CategoryComponent>::Get<T>();
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
    auto componentTypeIndex = TypeIndexGetter<World::CategoryComponent>::Get<T>();
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
}; // namespace ecs