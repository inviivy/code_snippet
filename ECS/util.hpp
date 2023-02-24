#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace ecs {
// Type map to uint32_t
struct TypeIndexGetter {
  template <typename T> static uint32_t Get() {
    static uint32_t id = curUniqueIdx_++;
    return id;
  }

private:
  inline static uint32_t curUniqueIdx_ = 0;
};

// Count id generate
template <typename CounterType>
requires(std::is_integral_v<CounterType>)
struct IdGenerator {
  static auto Gen() { return curIdIdx++; }

private:
  inline static CounterType curIdIdx = {};
};

}; // namespace ecs