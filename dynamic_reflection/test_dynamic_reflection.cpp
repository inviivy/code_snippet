#include "dynamic_reflection.hpp"
#include <iostream>
#include <string>

struct SimpleStruct {
  bool m_bool;
  int m_int;
  double m_double;
  std::string m_string;
};

int main() {
  auto bool_converter = [](bool *field, const std::string &name) {
    std::cout << "bool_converter:" << name << ":" << *field << '\n';
  };
  auto int_converter = [](int *field, const std::string &name) {
    std::cout << "int_converter:" << name << ":" << *field << '\n';
  };
  auto double_converter = [](double *field, const std::string &name) {
    std::cout << "double_converter:" << name << ":" << *field << '\n';
  };
  auto string_converter = [](std::string *field, const std::string &name) {
    std::cout << "string_converter:" << name << ":" << *field << '\n';
  };

  StructValueConverter<SimpleStruct> converter;
  converter.RegisterField(&SimpleStruct::m_bool, "bool",
                          ValueConverter<bool>(bool_converter));
  converter.RegisterField(&SimpleStruct::m_int, "int",
                          ValueConverter<int>(int_converter));
  converter.RegisterField(&SimpleStruct::m_double, "double",
                          ValueConverter<double>(double_converter));
  converter.RegisterField(&SimpleStruct::m_string, "string",
                          ValueConverter<std::string>(string_converter));
  SimpleStruct simple{true, 2, 2.0, "hello dynamic reflection"};
  converter(&simple);

  return 0;
}