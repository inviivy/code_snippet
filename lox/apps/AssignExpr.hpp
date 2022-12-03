#pragma once

#include "Expr.hpp"
#include "ExprVisitor.hpp"
#include "Token.hpp"

#include <memory>

namespace Lox {
class AssignExpr : public Expr {
  Token variable;
  std::unique_ptr<Expr> expr;

public:
  AssignExpr(const Token &, std::unique_ptr<Expr>);
  ~AssignExpr() = default;
  const Token &getVarName() const;
  const std::unique_ptr<Expr> &getExpr() const;
  virtual std::any accept(ExprVisitor<std::any> &) const override;
};
}; // namespace Lox