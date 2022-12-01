#pragma once
#include "Expr.hpp"
#include "Statement.hpp"
#include "Token.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

namespace Lox {
struct Parser {
  Parser(std::vector<Token> tokens);

  std::vector<std::unique_ptr<Statement>> parse();

private:
  bool isAtEnd() const;
  Token peek() const;
  Token previous() const;
  Token advance();
  Token expect(TokenType, const std::string &);

  bool match(TokenType type);

  struct ParseError : public std::runtime_error {
    ParseError() : std::runtime_error("") {}
  };

  static ParseError error(Token token, const char *message);

  std::unique_ptr<Statement> statement();
  std::unique_ptr<Statement> exprStatement();
  std::unique_ptr<Statement> printStatement();
  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> comparison();
  std::unique_ptr<Expr> addition();
  std::unique_ptr<Expr> multiplication();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> primary();

private:
  std::vector<Token> tokens;
  int current = 0;
};
}; // namespace Lox