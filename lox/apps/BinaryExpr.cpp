#include "BinaryExpr.hpp"

namespace Lox {
BinaryExpr::BinaryExpr(std::unique_ptr<Expr> left, Token oper,
                       std::unique_ptr<Expr> right)
    : left(std::move(left)), right(std::move(right)), op(oper) {}

std::any BinaryExpr::accept(ExprVisitor<std::any> &visitor) const {
  return visitor.visitBinaryExpr(*this);
}
}; // namespace Lox