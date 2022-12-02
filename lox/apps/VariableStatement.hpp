#pragma once

#include "Expr.hpp"
#include "Statement.hpp"
#include "Token.hpp"

#include <any>
#include <memory>

namespace Lox {
class VariableStatement : public Statement {
  Token varName;
  std::unique_ptr<Expr> expr;

public:
  VariableStatement(Token, std::unique_ptr<Expr>);
  ~VariableStatement() = default;
  std::any accept(StatementVisitor<std::any> &) const override;

  Token getVarName() const;
  const std::unique_ptr<Expr> &getInitializer() const;
};
}; // namespace Lox