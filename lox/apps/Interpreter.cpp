#include "Interpreter.hpp"
#include "BinaryExpr.hpp"
#include "Expr.hpp"
#include "GroupingExpr.hpp"
#include "LiteralExpr.hpp"
#include "RuntimeError.hpp"
#include "UnaryExpr.hpp"

#include <cmath>
#include <string>

namespace {
std::string stringify(const std::any &object) {
  if (!object.has_value()) {
    return "nil";
  }

  if (object.type() == typeid(double)) {
    double dvalue = std::any_cast<double>(object);
    if (std::trunc(dvalue) == dvalue) { // is int
      return std::to_string(int(dvalue));
    }
    return std::to_string(dvalue); // TODO: don't print trailing zeros
  }

  if (object.type() == typeid(std::string)) {
    return std::any_cast<std::string>(object);
  }
  return std::string{};
}
}; // namespace

namespace Lox {
bool Interpreter::isTruthy(const std::any &object) {
  if (!object.has_value()) {
    return false;
  }
  if (object.type() == typeid(bool)) {
    return std::any_cast<bool>(object);
  }
  return true;
}

bool Interpreter::isEqual(const std::any &left, const std::any &right) {
  if (!left.has_value() && !right.has_value()) {
    // 如果都为nil
    return true;
  }
  if (!left.has_value()) {
    return false;
  }

  if (left.type() != right.type()) {
    return false;
  }

  if (left.type() == typeid(bool)) {
    return std::any_cast<bool>(left) == std::any_cast<bool>(right);
  }

  if (left.type() == typeid(double)) {
    return std::any_cast<double>(left) == std::any_cast<double>(right);
  }

  if (left.type() == typeid(std::string)) {
    return std::any_cast<std::string>(left) ==
           std::any_cast<std::string>(right);
  }

  return false;
}

void Interpreter::checkNumberOperand(const Token &oper,
                                     const std::any &operand) {
  if (operand.type() == typeid(double)) {
    return;
  }
  throw RuntimeError{oper, "Operand must be a number"};
}
void Interpreter::checkNumberOperands(const Token &oper, const std::any &left,
                                      const std::any &right) {
  if (left.type() == typeid(double) && right.type() == typeid(double)) {
    return;
  }

  throw RuntimeError(oper, "Operands must be numbers");
}

std::any Interpreter::evaluate(const Expr &expr) { return expr.accept(*this); }

std::any Interpreter::visitGroupingExpr(const GroupingExpr &expr) {
  return evaluate(expr.getExpr());
}

std::any Interpreter::visitUnaryExpr(const UnaryExpr &expr) {
  auto right = evaluate(expr.getRightExpr());
  switch (expr.getOp().getType()) {
  case TokenType::Bang:
    return !isTruthy(right);
  case TokenType::Minus:
    checkNumberOperand(expr.getOp(), right);
    return -std::any_cast<double>(right);
  default:
    return std::any{};
  }
}

std::any Interpreter::visitBinaryExpr(const BinaryExpr &expr) {
  auto left = evaluate(expr.getLeftExpr());
  auto right = evaluate(expr.getRightExpr());
  switch (expr.getOp().getType()) {
  // !=
  case TokenType::BangEqual:
    return !isEqual(left, right);
  // ==
  case TokenType::EqualEqual:
    return isEqual(left, right);
  // >, only for double
  case TokenType::Greater:
    checkNumberOperands(expr.getOp(), left, right);
    return std::any_cast<double>(left) > std::any_cast<double>(right);
  // >=
  case TokenType::GreaterEqual:
    checkNumberOperands(expr.getOp(), left, right);
    return std::any_cast<double>(left) >= std::any_cast<double>(right);
  // <
  case TokenType::Less:
    checkNumberOperands(expr.getOp(), left, right);
    return std::any_cast<double>(left) < std::any_cast<double>(right);
  // <=
  case TokenType::LessEqual:
    checkNumberOperands(expr.getOp(), left, right);
    return std::any_cast<double>(left) <= std::any_cast<double>(right);
  case TokenType::Minus:
    checkNumberOperands(expr.getOp(), left, right);
    return std::any_cast<double>(left) - std::any_cast<double>(right);
  case TokenType::Plus:
    if (left.type() == typeid(double) && right.type() == typeid(double)) {
      return std::any_cast<double>(left) + std::any_cast<double>(right);
    }
    if (left.type() == typeid(std::string) &&
        right.type() == typeid(std::string)) {
      return std::any_cast<std::string>(left) +
             std::any_cast<std::string>(right);
    }

    throw RuntimeError(expr.getOp(), "Operands must be numbers or strings");

  case TokenType::Slash:
    checkNumberOperands(expr.getOp(), left, right);
    if (std::fabs(std::any_cast<double>(right)) <= 1e-15) {
      throw RuntimeError(expr.getOp(), "division zero");
    }
    return std::any_cast<double>(left) / std::any_cast<double>(right);

  case TokenType::Star:
    checkNumberOperands(expr.getOp(), left, right);
    return std::any_cast<double>(left) * std::any_cast<double>(right);
  default:
    return std::any{};
  }
}

std::any Interpreter::visitLiteralExpr(const LiteralExpr &expr) {
  return expr.getLiteral();
}

}; // namespace Lox