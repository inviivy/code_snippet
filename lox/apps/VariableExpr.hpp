#pragma once

#include "Expr.hpp"
#include "Token.hpp"

#include <any>

namespace Lox {
class VariableExpr : public Expr {
  Token varName;

public:
  VariableExpr(Token);
  ~VariableExpr() = default;
  std::any accept(ExprVisitor<std::any> &) const override;

  const Token &getVarName() const;
};
}; // namespace Lox