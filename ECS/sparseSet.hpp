#pragma once

#include <array>
#include <concepts>
#include <limits>
#include <type_traits>
#include <vector>

namespace ecs {
template <typename T, std::size_t PageSize>
requires(std::is_integral_v<T>)
struct SparseSet {
  void Add(T t) {
    density_.push_back(t);
    assure(t);
    index(t) = density_.size() - 1;
  }

  // 为了swap&&pop_back
  void Remove(T t) {
    if (!Contain(t))
      return;
    auto &idx = index(t);
    if (idx == density_.size() - 1) {
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
  T &index(T t) { return sparse_[page(t)][offset(t)]; }
  T index(T t) const { return sparse_[page(t)][offset(t)]; }
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
  std::vector<std::array<T, PageSize>> sparse_;
  constexpr static T null = std::numeric_limits<T>::max();
};
}; // namespace ecs