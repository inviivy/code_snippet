#include "Parser.hpp"
#include "BinaryExpr.hpp"
#include "GroupingExpr.hpp"
#include "LiteralExpr.hpp"
#include "Lox.hpp"
#include "UnaryExpr.hpp"

namespace Lox {
Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Expr>> Parser::parse() {
  std::vector<std::unique_ptr<Expr>> expressions;
  while (!isAtEnd()) {
    expressions.push_back(expression());
  }
  return expressions;
}

bool Parser::isAtEnd() const { return peek().getType() == TokenType::TokenEOF; }

Token Parser::peek() const { return tokens.at(current); }

Token Parser::previous() const { return tokens.at(current - 1); }

Token Parser::advance() {
  if (!isAtEnd()) {
    ++current;
  }
  return previous();
}

bool Parser::match(TokenType type) {
  if (isAtEnd()) {
    return false;
  }
  if (peek().getType() == type) {
    advance();
    return true;
  }
  return false;
}

std::unique_ptr<Expr> Parser::expression() { return addition(); }

std::unique_ptr<Expr> Parser::addition() {
  auto expr = multiplication();
  while (match(TokenType::Minus) || match(TokenType::Plus)) {
    auto oper = previous();
    auto right = multiplication();
    expr =
        std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
  }
  return expr;
}

std::unique_ptr<Expr> Parser::multiplication() {
  auto expr = unary();
  while (match(TokenType::Slash) || match(TokenType::Star)) {
    auto oper = previous();
    auto right = unary();
    expr =
        std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::unary() {
  if (match(TokenType::Bang) || match(TokenType::Minus)) {
    auto oper = previous();
    auto right = unary();
    return std::make_unique<UnaryExpr>(oper, std::move(right));
  }

  return primary();
}

std::unique_ptr<Expr> Parser::primary() {
  if (match(TokenType::Number)) {
    return std::make_unique<LiteralExpr>(previous().getLiteral());
  }

  if (match(TokenType::LeftParen)) {
    auto expr = expression();
    // 必须有右括号
    if (peek().getType() == TokenType::RightParen) {
      advance();
      return std::make_unique<GroupingExpr>(std::move(expr));
    }
    throw error(peek(), "Expect ')' after expression");
  }

  throw error(peek(), "Expect expression.");
}

Parser::ParseError Parser::error(Token token, const char *message) {
  Lox::Error(token, message);
  throw ParseError{};
}

}; // namespace Lox