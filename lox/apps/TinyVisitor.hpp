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
    return std::any_cast<double>(expr.getExpr().accept(*this));
  }

  std::any visitUnaryExpr(const UnaryExpr &expr) override {
    auto opType = expr.getOp().getType();
    if (opType == TokenType::Minus) {
      return std::any_cast<double>(expr.getRightExpr().accept(*this)) * (-1.0);
    }
    return double(0.0);
  }

  std::any visitBinaryExpr(const BinaryExpr &expr) override {
    auto opType = expr.getOp().getType();
    auto left = expr.getLeftExpr().accept(*this);
    auto right = expr.getRightExpr().accept(*this);
    switch (opType) {
    case TokenType::Plus:
      return std::any_cast<double>(left) + std::any_cast<double>(right);
    case TokenType::Minus:
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::Star:
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    case TokenType::Slash:
      if (std::any_cast<double>(right) == 0) {
        throw std::runtime_error("division 0");
      }
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    default:
      return double{};
    };
  }

  std::any visitLiteralExpr(const LiteralExpr &expr) override {
    return std::any_cast<double>(expr.getLiteral());
  }
};
}; // namespace Lox