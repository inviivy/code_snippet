#include "GroupingExpr.hpp"

namespace Lox {
GroupingExpr::GroupingExpr(std::unique_ptr<Expr> expr)
    : expr(std::move(expr)) {}

std::any GroupingExpr::accept(ExprVisitor<std::any> &visitor) const {
  return visitor.visitGroupingExpr(*this);
}
}; // namespace Lox