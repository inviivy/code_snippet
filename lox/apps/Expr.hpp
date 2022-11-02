#pragma once
#include "ExprVisitor.hpp"
#include <any>

namespace Lox {
struct Expr {
  virtual ~Expr() = default;
  virtual std::any accept(ExprVisitor<std::any> &visitor) const = 0;
};
}; // namespace Lox