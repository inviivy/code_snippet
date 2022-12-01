#include "Parser.hpp"
#include "BinaryExpr.hpp"
#include "ExpressionStatement.hpp"
#include "GroupingExpr.hpp"
#include "LiteralExpr.hpp"
#include "Lox.hpp"
#include "PrintStatement.hpp"
#include "UnaryExpr.hpp"

namespace Lox {
Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

bool Parser::isAtEnd() const { return peek().getType() == TokenType::TokenEOF; }

Token Parser::peek() const { return tokens.at(current); }

Token Parser::previous() const { return tokens.at(current - 1); }

Token Parser::advance() {
  if (!isAtEnd()) {
    ++current;
  }
  return previous();
}

Token Parser::expect(TokenType type, const std::string &msg) {
  if (match(type)) {
    return advance();
  }
  /* 如何处理错误 */
  Lox::Error(peek(), msg);
  throw ParseError();
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

std::vector<std::unique_ptr<Statement>> Parser::parse() {
  // program         →   statement* EOF
  std::vector<std::unique_ptr<Statement>> statements;
  while (!isAtEnd()) {
    statements.push_back(statement());
  }
  return statements;
}

std::unique_ptr<Statement> Parser::statement() {
  // statement       →   exprStmt | printStmt
  try {
    if (match(TokenType::Print)) {
      return printStatement();
    }
    return exprStatement();
  } catch (ParseError error) {
    return nullptr;
  }
}

std::unique_ptr<Statement> Parser::printStatement() {
  auto expr = expression();
  expect(TokenType::Semicolon, std::string("Expect ';' after value"));
  return std::make_unique<PrintStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::exprStatement() {
  auto expr = expression();
  expect(TokenType::Semicolon, std::string("Expect ';' after value"));
  return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Expr> Parser::expression() { return equality(); }

std::unique_ptr<Expr> Parser::equality() {
  // equality        →   comparison ( ( "!=" | "==" ) comparison ) *
  auto expr = comparison();
  while (match(TokenType::BangEqual) || match(TokenType::EqualEqual)) {
    auto oper = previous();
    auto right = comparison();
    expr =
        std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
  }
  return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
  // comparison      →   addition ( ( ">" | ">=" | "<" | "<=" ) addition ) *
  auto expr = addition();
  while (match(TokenType::Greater) || match(TokenType::GreaterEqual) ||
         match(TokenType::Less) || match(TokenType::LessEqual)) {
    auto oper = previous();
    auto right = addition();
    expr =
        std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::addition() {
  // addition        →   multiplication ( ( "-" | "+" ) multiplication ) *
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
  // multiplication  →   unary ( ( "/" | "*" ) unary ) *
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
  // unary → ( "!" | "-" ) unary
  if (match(TokenType::Bang) || match(TokenType::Minus)) {
    auto oper = previous();
    auto right = unary();
    return std::make_unique<UnaryExpr>(oper, std::move(right));
  }

  return primary();
}

std::unique_ptr<Expr> Parser::primary() {
  // primary         →   NUMBER | STRING | "true" | "false" | "nil" | "("
  // expression ")"
  if (match(TokenType::Number) || match(TokenType::String)) {
    return std::make_unique<LiteralExpr>(previous().getLiteral());
  }

  if (match(TokenType::False)) {
    return std::make_unique<LiteralExpr>(false);
  }

  if (match(TokenType::True)) {
    return std::make_unique<LiteralExpr>(true);
  }

  if (match(TokenType::Nil)) {
    return std::make_unique<LiteralExpr>(std::any{});
  }

  if (match(TokenType::LeftParen)) {
    auto expr = expression();
    // 必须有右括号, 否则抛出异常
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