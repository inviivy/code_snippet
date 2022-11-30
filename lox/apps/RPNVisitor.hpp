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
struct RPNVisitor : public ExprVisitor<std::any> {
  std::any visitGroupingExpr(const GroupingExpr &expr) override {
    return std::any_cast<std::string>(expr.getExpr().accept(*this));
  }

  std::any visitUnaryExpr(const UnaryExpr &expr) override {
    auto opType = expr.getOp().getType();
    if (opType == TokenType::Minus) {
      auto dstr = std::stod(
          std::any_cast<std::string>(expr.getRightExpr().accept(*this)));
      return std::to_string(dstr * (-1.0));
    }
    return std::string("0.0");
  }

  std::any visitBinaryExpr(const BinaryExpr &expr) override {
    auto opType = expr.getOp().getType();
    auto left = expr.getLeftExpr().accept(*this);
    auto right = expr.getRightExpr().accept(*this);
    switch (opType) {
    case TokenType::Plus:
      return fmt::format("{} {} {}", std::any_cast<std::string>(left),
                         std::any_cast<std::string>(right), std::string("+"));
    case TokenType::Minus:
      return fmt::format("{} {} {}", std::any_cast<std::string>(left),
                         std::any_cast<std::string>(right), std::string("-"));
    case TokenType::Star:
      return fmt::format("{} {} {}", std::any_cast<std::string>(left),
                         std::any_cast<std::string>(right), std::string("*"));
    case TokenType::Slash:
      if (std::stod(std::any_cast<std::string>(right)) == 0.0) {
        throw std::runtime_error("division 0");
      }
      return fmt::format("{} {} {}", std::any_cast<std::string>(left),
                         std::any_cast<std::string>(right), std::string("/"));
    default:
      return fmt::format("");
    };
  }

  std::any visitLiteralExpr(const LiteralExpr &expr) override {
    return std::to_string(std::any_cast<double>(expr.getLiteral()));
  }
};
}; // namespace Lox