#pragma once

#include "ExprVisitor.hpp"
#include "Token.hpp"

#include <any>

namespace Lox {

class Expr;

class Interpreter : public ExprVisitor<std::any> {
public:
  ~Interpreter();
  std::any evaluate(const Expr &);

  void interpret();

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
};
}; // namespace Lox