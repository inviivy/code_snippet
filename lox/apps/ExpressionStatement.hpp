#pragma once

#include "Expr.hpp"
#include "Statement.hpp"
#include "StatementVisitor.hpp"

#include <memory>

namespace Lox {
struct ExpressionStatement : public Statement {
  ExpressionStatement(std::unique_ptr<Expr>);
  std::any accept(StatementVisitor<std::any> &) const override;

  const Expr &getExpr() const { return *expr; }

private:
  std::unique_ptr<Expr> expr;
};
}; // namespace Lox