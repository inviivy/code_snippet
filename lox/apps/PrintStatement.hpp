#pragma once

#include "Expr.hpp"
#include "Statement.hpp"
#include "StatementVisitor.hpp"

#include <memory>

namespace Lox {
struct PrintStatement : public Statement {
  PrintStatement(std::unique_ptr<Expr>);
  std::any accept(StatementVisitor<std::any> &) const override;

  const Expr &getExpr() const { return *expr; }

private:
  std::unique_ptr<Expr> expr;
};
}; // namespace Lox