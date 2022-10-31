#include "Singleton.hpp"

struct MyType {
  MyType(int, double, float) {}
};

int main() {
  Singleton<MyType>::Instance(1, 1.0, 1.0f);
  auto ptr = Singleton<MyType>::GetInstance();
  return 0;
}