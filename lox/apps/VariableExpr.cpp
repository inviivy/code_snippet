#include "VariableExpr.hpp"

namespace Lox {
VariableExpr::VariableExpr(Token varName) : varName(std::move(varName)) {}

std::any VariableExpr::accept(ExprVisitor<std::any> &visitor) const {
  return visitor.visitVariableExpr(*this);
}

const Token &VariableExpr::getVarName() const { return varName; }
}; // namespace Lox