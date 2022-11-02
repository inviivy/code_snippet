#pragma once
#include "Expr.hpp"
#include "Token.hpp"
#include <memory>

namespace Lox {
struct BinaryExpr : public Expr {
  explicit BinaryExpr(std::unique_ptr<Expr> left, Token op,
                      std::unique_ptr<Expr> right);
  std::any accept(ExprVisitor<std::any> &visitor) const override;

  const Expr &getLeftExpr() const { return *left; }
  const Expr &getRightExpr() const { return *right; }
  const Token &getOp() const { return op; }

private:
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  Token op;
};
}; // namespace Lox