#pragma once
#include "BinaryExpr.hpp"
#include "ExprVisitor.hpp"
#include "GroupingExpr.hpp"
#include "LiteralExpr.hpp"
#include "UnaryExpr.hpp"
#include <any>
#include <fmt/core.h>
#include <fmt/format.h>
#include <string>

namespace Lox {
struct TinyVisitor : public ExprVisitor<std::any> {

  std::any visitGroupingExpr(const GroupingExpr &expr) override {
    return fmt::format(
        "({})", std::any_cast<std::string>(expr.getExpr().accept(*this)));
  }

  std::any visitUnaryExpr(const UnaryExpr &expr) override {
    auto opType = expr.getOp().getType();
    std::string opStr;
    if (opType == TokenType::Minus) {
      opStr = "-";
    }
    return fmt::format(
        "{}{}", opStr,
        std::any_cast<std::string>(expr.getRightExpr().accept(*this)));
  }

  std::any visitBinaryExpr(const BinaryExpr &expr) override {
    auto opType = expr.getOp().getType();
    std::string opStr;
    switch (opType) {
    case TokenType::Plus:
      opStr = "+";
      break;
    case TokenType::Minus:
      opStr = "-";
      break;
    case TokenType::Star:
      opStr = "*";
      break;
    case TokenType::Slash:
      opStr = "/";
      break;
    default:
      break;
    };
    /* 目前只支持+-*\/四个符号 */
    return fmt::format(
        "{}{}{}", std::any_cast<std::string>(expr.getLeftExpr().accept(*this)),
        opStr, std::any_cast<std::string>(expr.getRightExpr().accept(*this)));
  }

  std::any visitLiteralExpr(const LiteralExpr &expr) override {
    return std::to_string(std::any_cast<double>(expr.getLiteral()));
  }
};
}; // namespace Lox