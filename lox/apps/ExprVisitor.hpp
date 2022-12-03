#pragma once

namespace Lox {
class GroupingExpr;
class UnaryExpr;
class BinaryExpr;
class LiteralExpr;
class VariableExpr;
class AssignExpr;

template <typename R> struct ExprVisitor {
  ~ExprVisitor() = default;
  virtual R visitGroupingExpr(const GroupingExpr &expr) = 0;
  virtual R visitUnaryExpr(const UnaryExpr &expr) = 0;
  virtual R visitBinaryExpr(const BinaryExpr &expr) = 0;
  virtual R visitLiteralExpr(const LiteralExpr &expr) = 0;
  virtual R visitVariableExpr(const VariableExpr &expr) = 0;
  virtual R visitAssignExpr(const AssignExpr &expr) = 0;
};
}; // namespace Lox