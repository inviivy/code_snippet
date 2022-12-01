#pragma once

namespace Lox {
class PrintStatement;
class ExpressionStatement;

template <typename T> struct StatementVisitor {
  ~StatementVisitor() = default;
  virtual T visitExpressionStmt(const ExpressionStatement &) = 0;
  virtual T visitPrintStatement(const PrintStatement &) = 0;
};
}; // namespace Lox