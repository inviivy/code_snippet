#pragma once
#include "Expr.hpp"
#include "ExprVisitor.hpp"
#include "Token.hpp"
#include <memory>

namespace Lox {
struct GroupingExpr : public Expr {
  GroupingExpr(std::unique_ptr<Expr> expr);
  std::any accept(ExprVisitor<std::any> &visitor) const override;

  const Expr &getExpr() const { return *expr; }

private:
  std::unique_ptr<Expr> expr;
};
}; // namespace Lox