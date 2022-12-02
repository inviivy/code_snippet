#pragma once

namespace Lox {
class PrintStatement;
class ExpressionStatement;
class VariableStatement;

template <typename T> struct StatementVisitor {
  ~StatementVisitor() = default;
  virtual T visitExpressionStmt(const ExpressionStatement &) = 0;
  virtual T visitPrintStatement(const PrintStatement &) = 0;
  virtual T visitVariableStatement(const VariableStatement &) = 0;
};
}; // namespace Lox