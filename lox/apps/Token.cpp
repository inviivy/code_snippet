#include "Token.hpp"

namespace Lox {
Token::Token(TokenType type, std::string lexeme, int line)
    : type(type), lexeme(std::move(lexeme)), line(line) {}
Token::Token(TokenType type, std::string lexeme, std::any literal, int line)
    : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)),
      line(line) {}

std::string Token::toString() const {
  return std::to_string(static_cast<int>(type)) + ", lexeme: '" + lexeme +
         "', literal: '" + literalToString() + "'";
}

std::string Token::literalToString() const {
  switch (type) {
  case TokenType::String:
    return std::any_cast<std::string>(literal);
  case TokenType::Number:
    return std::to_string(std::any_cast<double>(literal));
  default:
    return "";
  }
}

}; // namespace Lox