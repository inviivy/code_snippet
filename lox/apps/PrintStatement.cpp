#include "PrintStatement.hpp"

namespace Lox {
PrintStatement::PrintStatement(std::unique_ptr<Expr> expr)
    : expr(std::move(expr)) {}

std::any PrintStatement::accept(StatementVisitor<std::any> &visitor) const {
  return visitor.visitPrintStatement(*this);
}
}; // namespace Lox