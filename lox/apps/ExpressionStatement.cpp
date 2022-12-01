#include "ExpressionStatement.hpp"

namespace Lox {
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expr> expr)
    : expr(std::move(expr)) {}

std::any
ExpressionStatement::accept(StatementVisitor<std::any> &visitor) const {
  return visitor.visitExpressionStmt(*this);
}
}; // namespace Lox