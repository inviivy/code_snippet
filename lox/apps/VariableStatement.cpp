#include "VariableStatement.hpp"

namespace Lox {
VariableStatement::VariableStatement(Token token, std::unique_ptr<Expr> expr)
    : varName(std::move(token)), expr(std::move(expr)) {}

std::any VariableStatement::accept(StatementVisitor<std::any> &visitor) const {
  return visitor.visitVariableStatement(*this);
}

Token VariableStatement::getVarName() const { return varName; }

const std::unique_ptr<Expr> &VariableStatement::getInitializer() const {
  return expr;
}

}; // namespace Lox