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

struct World {
  friend class Commands;
  friend class Resource;

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
  struct Resource {};
  struct Component {};

private:
  ComponentMap componentMap_;
  std::unordered_map<Entity, ComponentContainer> entities_;
  /* 一个类型的resource只允许存在一个, 比如Timer，Render等. */
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
    auto resourceTypeIndex = TypeIndexGetter<World::Resource>::Get<T>();
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
    auto resourceTypeIndex = TypeIndexGetter<World::Resource>::Get<T>();
    world_.resources_.erase(resourceTypeIndex);
    return *this;
  }

private:
  template <typename T, typename... Remains>
  void do_spawn(Entity entity, T &&component, Remains &&...remains) {
    // 类型index

    auto componentTypeIndex = TypeIndexGetter<World::Component>::Get<T>();
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
    auto resourceTypeIndex = TypeIndexGetter<World::Resource>::Get<T>();
    return world_.resources_.find(resourceTypeIndex) != world_.resources_.end();
  }

  template <typename T>
  requires(!std::is_reference_v<T> && !std::is_pointer_v<T>)
  T &Get() {
    auto resourceTypeIndex = TypeIndexGetter<World::Resource>::Get<T>();
    // non error handle
    return *static_cast<T *>(world_.resources_[resourceTypeIndex].source_);
  }

private:
  World &world_;
};
}; // namespace ecs