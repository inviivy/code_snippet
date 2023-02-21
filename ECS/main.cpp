#include "sparseSet.hpp"

#include <cstdint>

int main() {
  ecs::SparseSet<uint32_t, 8> sparseSet;
  for(uint32_t i = 0; i < 2048; ++i) {
    sparseSet.Add(i);
  }
  for(uint32_t i = 0; i < 2048; ++i) {
    sparseSet.Contain(i);
  }
  return 0;
}