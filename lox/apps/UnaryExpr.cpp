#include "UnaryExpr.hpp"

namespace Lox {
UnaryExpr::UnaryExpr(Token oper, std::unique_ptr<Expr> right)
    : op(std::move(oper)), right(std::move(right)) {}

std::any UnaryExpr::accept(ExprVisitor<std::any> &visitor) const {
  return visitor.visitUnaryExpr(*this);
}
}; // namespace Lox