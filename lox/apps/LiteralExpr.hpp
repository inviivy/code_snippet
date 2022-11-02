#pragma once
#include "Expr.hpp"
#include "ExprVisitor.hpp"
#include "Token.hpp"
#include <memory>

namespace Lox {
struct LiteralExpr : public Expr {
  LiteralExpr(std::any literal);
  std::any accept(ExprVisitor<std::any> &visitor) const override;

  const std::any &getLiteral() const { return literal; }

private:
  std::any literal;
};
}; // namespace Lox