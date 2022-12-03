#include "AssignExpr.hpp"

namespace Lox {

AssignExpr::AssignExpr(const Token &leftVariable, std::unique_ptr<Expr> expr)
    : variable(leftVariable), expr(std::move(expr)) {}

std::any AssignExpr::accept(ExprVisitor<std::any> &visitor) const {
  return visitor.visitAssignExpr(*this);
}

const Token &AssignExpr::getVarName() const { return variable; }

const std::unique_ptr<Expr> &AssignExpr::getExpr() const { return expr; }

}; // namespace Lox