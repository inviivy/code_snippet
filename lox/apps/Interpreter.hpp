#pragma once

#include "Environment.hpp"
#include "ExprVisitor.hpp"
#include "StatementVisitor.hpp"
#include "Token.hpp"

#include <any>
#include <memory>
#include <vector>

namespace Lox {

class Statement;
class Expr;

class Interpreter : public ExprVisitor<std::any>,
                    public StatementVisitor<std::any> {
  std::unique_ptr<Environment> environment;

public:
  Interpreter();
  ~Interpreter() = default;
  void execute(const Statement &);
  std::any evaluate(const Expr &);

  void interpret(const std::vector<std::unique_ptr<Statement>> &);

private:
  static bool isTruthy(const std::any &);
  static bool isEqual(const std::any &, const std::any &);

  static void checkNumberOperand(const Token &oper, const std::any &operand);
  static void checkNumberOperands(const Token &oper, const std::any &left,
                                  const std::any &right);

  std::any visitGroupingExpr(const GroupingExpr &) override;
  std::any visitUnaryExpr(const UnaryExpr &) override;
  std::any visitBinaryExpr(const BinaryExpr &) override;
  std::any visitLiteralExpr(const LiteralExpr &) override;
  std::any visitVariableExpr(const VariableExpr &) override;

  std::any visitExpressionStmt(const ExpressionStatement &) override;
  std::any visitPrintStatement(const PrintStatement &) override;
  std::any visitVariableStatement(const VariableStatement &) override;
};
}; // namespace Lox