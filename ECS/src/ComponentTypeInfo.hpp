#include "sparseSet.hpp"

namespace ecs {

struct ComponentTypeInfo {
  using CreateFunc = void *(*)();
  using DestroyFunc = void (*)(void *);

  CreateFunc createFn_ = nullptr;
  DestroyFunc destroyFn_ = nullptr;
  // pageSize = 32
  SparseSet<uint32_t, 32> sparseSet_;

  ComponentTypeInfo() {}
  ~ComponentTypeInfo() {}
  ComponentTypeInfo(CreateFunc createFn, DestroyFunc destroyFn)
      : createFn_(createFn), destroyFn_(destroyFn) {}

  ComponentTypeInfo(const ComponentTypeInfo &) = delete;
  ComponentTypeInfo &operator=(const ComponentTypeInfo &) = delete;

  ComponentTypeInfo(ComponentTypeInfo &&info) noexcept
      : createFn_(info.createFn_), destroyFn_(info.destroyFn_),
        sparseSet_(std::move(info.sparseSet_)) {}

  ComponentTypeInfo &operator=(ComponentTypeInfo &&info) noexcept {
    createFn_ = info.createFn_;
    destroyFn_ = info.destroyFn_;
    sparseSet_ = std::move(info.sparseSet_);
    return *this;
  }
};
}; // namespace ecs