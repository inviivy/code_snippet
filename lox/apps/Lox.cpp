#include "Lox.hpp"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <iostream>

namespace Lox {
bool Lox::HadError = false;
bool Lox::HadRuntimeError = false;

void Lox::Report(int line, const std::string &where,
                 const std::string &message) {
  fmt::print(std::cerr, "[line {}] Error{}: {}\n", line, where, message);
}

void Lox::Error(int line, const std::string &message) {
  Report(line, "", message);
  HadError = true;
}
}; // namespace Lox