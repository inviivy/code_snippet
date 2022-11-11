#include "IoC.hpp"
#include <iostream>

struct Base {
  virtual void func() {}
  virtual ~Base() {}
};

struct DerivedA : public Base {
  DerivedA(int a, int b) : a(a), b(b) {}
  void func() override { std::cout << a + b << '\n'; }

private:
  int a;
  int b;
};

struct DerivedB : public Base {};

// A依赖于Base
struct A {
  A() {}
  A(Base *ptr) : m_ptr{ptr} {}

  /* A call interface */
  void func() { m_ptr->func(); }

  ~A() {
    if (m_ptr) {
      delete m_ptr;
      m_ptr = nullptr;
    }
  }

private:
  Base *m_ptr = nullptr;
};

int main() {
  IoContainer ioc;
  // should using xml or json configurate?
  // condition_a -> DerivedA
  ioc.RegisterType<A, DerivedA, int, int>(std::string("condition_A"));
  ioc.RegisterType<A, DerivedB>(std::string("condition_B"));

  auto a = ioc.ResolveShared<A, int, int>(std::string("condition_A"), 1, 2);
  a->func();
  return 0;
}