#pragma once

#include "Token.hpp"
#include <unordered_map>
#include <vector>
#include <string_view>

namespace Lox {
struct Scanner {
  Scanner(std::string source);
  std::vector<Token> scanTokens();

private:
  bool isAtEnd() const;
  char advance();
  std::string_view advance(size_t n);
  bool match(char expected);
  char peek() const;
  std::string_view peekUtf8() const;
  char peekNext() const;

  void addToken(TokenType type, std::any literal);
  void addToken(TokenType type);

  void comment();
  void string();
  void number();
  void identifier();
  void scanToken();

  bool isUtf8();

private:
  std::string source;
  std::vector<Token> tokens;
  std::unordered_map<std::string, TokenType> keywords;

  // 对source遍历的状态
  int start{0};
  int current{0};
  int line{1};
};
}; // namespace Lox