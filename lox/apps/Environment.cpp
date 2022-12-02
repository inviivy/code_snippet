#include "Environment.hpp"
#include "RuntimeError.hpp"

#include <fmt/core.h>

namespace Lox {
Environment::Environment() {}

Environment::~Environment() {}

void Environment::define(const std::string &name, const std::any &value) {
  values.insert_or_assign(name, value);
}
void Environment::assign(const Token &token, const std::any &value) {
  if (auto iter = values.find(token.getLexeme()); iter != values.end()) {
    iter->second = value;
    return;
  }

  throw RuntimeError(token,
                     fmt::format("Undefine variable {}", token.getLexeme()));
}

const std::any &Environment::getVal(const Token &token) const {
  if (auto iter = values.find(token.getLexeme()); iter != values.end()) {
    return iter->second;
  }
  throw RuntimeError(token,
                     fmt::format("Undefine variable {}", token.getLexeme()));
}

}; // namespace Lox