#pragma once

#include "StatementVisitor.hpp"

#include <any>

namespace Lox {

struct Statement {
  virtual ~Statement() = default;
  virtual std::any accept(StatementVisitor<std::any> &) const = 0;
};
}; // namespace Lox