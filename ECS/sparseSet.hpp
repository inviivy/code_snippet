#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

namespace ecs {
template <typename T, std::size_t PageSize>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
struct SparseSet {
  SparseSet() {}
  ~SparseSet() {}
  SparseSet(const SparseSet &) = delete;
  SparseSet &operator=(const SparseSet &) = delete;

  SparseSet(SparseSet &&set) noexcept
      : density_(std::move(set.density_)), sparse_(std::move(set.sparse_)) {}

  SparseSet &operator=(SparseSet &&set) noexcept {
    density_ = std::move(set.density_);
    sparse_ = std::move(set.sparse_);
    return *this;
  }

  void Add(T t) {
    // 避免重复添加
    if (Contain(t)) {
      return;
    }
    density_.push_back(t);
    assure(t);
    // maybe narrow convert
    index(t) = static_cast<uint32_t>(density_.size() - 1);
  }

  void Remove(T t) {
    if (!Contain(t))
      return;
    auto &idx = index(t);
    if (idx == static_cast<uint32_t>(density_.size() - 1)) {
      idx = null;
      density_.pop_back();
    } else {
      auto last = density_.back();
      index(last) = idx; // 因为要把这个key放到原来放t的位置上
      std::swap(density_[idx], density_.back());
      idx = null; // 删除了key所以它对应的下标应该为null
      density_.pop_back();
    }
  }

  bool Contain(T t) const {
    auto p = page(t);
    auto o = offset(t);
    return p < sparse_.size() && sparse_[p][o] != null;
  }

private:
  uint32_t &index(T t) { return sparse_[page(t)][offset(t)]; }
  uint32_t index(T t) const { return sparse_[page(t)][offset(t)]; }
  std::size_t page(T t) const { return t / PageSize; }
  std::size_t offset(T t) const { return t % PageSize; }

  /* 扩容*/
  void assure(T t) {
    auto p = page(t);
    if (p >= sparse_.size()) {
      for (std::size_t i = sparse_.size(); i <= p; ++i) {
        sparse_.emplace_back();
        sparse_[i].fill(null);
      }
    }
  }

private:
  std::vector<T> density_;
  // 存储的是线性表的下标, 这里使用uint32_t代替
  std::vector<std::array<uint32_t, PageSize>> sparse_;
  inline static uint32_t null = std::numeric_limits<uint32_t>::max();
};
}; // namespace ecs