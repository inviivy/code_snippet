#include <iostream>
#include <memory>

struct A {
  A() {}
  A(const A &) = delete;
  A &operator=(const A &) = delete;
  A(A &&rhs) noexcept : str(std::move(rhs.str)) {}
  A &operator=(A &&rhs) noexcept {
    str = std::move(rhs.str);
    return *this;
  }

  // private:
  std::unique_ptr<std::string> str;
};

struct B : public A {
  B() {}
  B(const B &) = delete;
  B &operator=(const B &) = delete;

  B(B &&rhs) noexcept : A{std::move(rhs)}, ptr{std::move(rhs.ptr)} {}

  B &operator=(B &&rhs) noexcept {
    A::operator=(std::move(rhs));
    ptr = std::move(rhs.ptr);
    return *this;
  }

  // private:
  std::unique_ptr<int> ptr;
};

int main() {
  B b1, b2;
  b1.str = std::make_unique<std::string>(32, 'a');
  b1.ptr = std::make_unique<int>(100);
  std::cout << *b1.str << '\t' << *b1.ptr << '\n';
  b2 = std::move(b1);
  std::cout << *b2.str << '\t' << *b2.ptr << '\n';
}