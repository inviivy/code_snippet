#pragma once

#include "ComponentTypeInfo.hpp"
#include "util.hpp"

#include <cassert>
#include <cstdint>
#include <unordered_map>

namespace ecs {
using ComponentTypeID = uint32_t;
using Entity = uint32_t;

struct Commands;

struct World {
  friend class Commands;

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
  ComponentMap componentMap_;
  std::unordered_map<Entity, ComponentContainer> entities_;
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

private:
  template <typename T, typename... Remains>
  void do_spawn(Entity entity, T &&component, Remains &&...remains) {
    // 类型index
    auto componentTypeIndex = TypeIndexGetter::Get<T>();
    if (auto it = world_.componentMap_.find(componentTypeIndex);
        it == world_.componentMap_.end()) {
      // 插入Component的构造和析构函数
      world_.componentMap_.emplace(
          componentTypeIndex,
          ComponentTypeInfo([]() -> void * { return new T; },
                            [](void *elem) { delete static_cast<T *>(elem); }));
    }
    ComponentTypeInfo &componentTypeInfo =
        world_.componentMap_[componentTypeIndex];
    void *element = componentTypeInfo.createFn_();
    *static_cast<T *>(element) = std::forward<T>(component);
    componentTypeInfo.sparseSet_.Add(entity);

    if (auto it = world_.entities_.find(entity); it == world_.entities_.end()) {
      auto [componentIt, succ] =
          world_.entities_.emplace(entity, World::ComponentContainer{});
      componentIt->second[componentTypeIndex] = element;
    } else {
      it->second[componentTypeIndex] = element;
    }

    if constexpr (sizeof...(remains) != 0) {
      do_spawn(entity, std::forward<Remains>(remains)...);
    }
  }

private:
  World &world_;
};
}; // namespace ecs