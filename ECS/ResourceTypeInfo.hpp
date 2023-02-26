#pragma once

#include <cassert>
#include <utility>

#include <iostream>

/* Resource满足RAII */
struct ResourceTypeInfo {
  using CreateFunc = void *(*)();
  using DestroyFunc = void (*)(void *);

  CreateFunc createFn_ = nullptr;
  DestroyFunc destroyFn_ = nullptr;
  void *source_ = nullptr;

  ResourceTypeInfo() {}

  ~ResourceTypeInfo() {
    if (source_) {
      assert(destroyFn_ != nullptr);
      destroyFn_(source_);
    }
  }
  ResourceTypeInfo(CreateFunc createFn, DestroyFunc destroyFn)
      : createFn_(createFn), destroyFn_(destroyFn) {}

  ResourceTypeInfo(const ResourceTypeInfo &) = delete;
  ResourceTypeInfo &operator=(const ResourceTypeInfo &) = delete;

  ResourceTypeInfo(ResourceTypeInfo &&info) noexcept
      : createFn_(info.createFn_), destroyFn_(info.destroyFn_),
        source_(std::exchange(info.source_, nullptr)) {}

  ResourceTypeInfo &operator=(ResourceTypeInfo &&info) noexcept {
    createFn_ = info.createFn_;
    destroyFn_ = info.destroyFn_;
    source_ = std::exchange(info.source_, nullptr);
    return *this;
  }
};