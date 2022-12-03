#include "Parser.hpp"
#include "AssignExpr.hpp"
#include "BinaryExpr.hpp"
#include "ExpressionStatement.hpp"
#include "GroupingExpr.hpp"
#include "LiteralExpr.hpp"
#include "Lox.hpp"
#include "PrintStatement.hpp"
#include "UnaryExpr.hpp"
#include "VariableExpr.hpp"
#include "VariableStatement.hpp"

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
    return previous();
  }
  /* 如何处理错误 */
  Lox::Error(peek(), msg);
  throw ParseError();
}

void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().getType() == TokenType::Semicolon) {
      return;
    }
    switch (peek().getType()) {
    case TokenType::Print:
      return;
    default:
      break;
    }
    advance();
  }
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
    statements.push_back(declaration());
  }
  return statements;
}

std::unique_ptr<Statement> Parser::declaration() {
  try {
    if (match(TokenType::Var)) {
      return varDecl();
    }
    return statement();
  } catch (ParseError error) {
    synchronize();
    return nullptr;
  }
}

std::unique_ptr<Statement> Parser::varDecl() {
  auto varName =
      expect(TokenType::Identifier, std::string("Expect variable name."));
  std::unique_ptr<Expr> initializer;
  if (match(TokenType::Equal)) {
    initializer = expression();
  }
  expect(TokenType::Semicolon, "Expect ';' after variable declaration.");
  // 返回varStatement
  return std::make_unique<VariableStatement>(std::move(varName),
                                             std::move(initializer));
}

std::unique_ptr<Statement> Parser::statement() {
  // statement       →   exprStmt | printStmt
  if (match(TokenType::Print)) {
    return printStatement();
  }
  return exprStatement();
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

std::unique_ptr<Expr> Parser::expression() { return assignment(); }

std::unique_ptr<Expr> Parser::assignment() {
  auto expr = equality();
  if (match(TokenType::Equal)) {
    auto equals = previous();
    auto rightExpr = assignment();
    /* 如果是等于号, 那么就要判断上下文 */
    try {
      if (auto *left = dynamic_cast<VariableExpr *>(expr.get());
          left != nullptr) {
        return std::make_unique<AssignExpr>(left->getVarName(),
                                            std::move(rightExpr));
      }
    } catch (std::bad_cast) {
      error(equals, "Invalid assignment target.");
    }
  }

  return expr;
}

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

  if (match(TokenType::Identifier)) {
    /* 返回identifier */
    return std::make_unique<VariableExpr>(previous());
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