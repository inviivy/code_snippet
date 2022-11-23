#include "Scanner.hpp"
#include "Lox.hpp"

#include <fmt/core.h>

#include <cctype>

namespace Lox {
Scanner::Scanner(std::string source) : source(std::move(source)) {
  /**
   * @brief 初始化所有的keywords解决fori先匹配for的问题
   * 暂时只有这几个关键字
   * */
  keywords = {{"and", TokenType::And},
              {"var", TokenType::Var},
              {"nil", TokenType::Nil},
              {"print", TokenType::Print}};
}

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }
  // 显示增加一个EOF token作为结束
  tokens.emplace_back(TokenType::TokenEOF, "", line);
  return tokens;
}

bool Scanner::isAtEnd() const {
  return current >= static_cast<int>(source.size());
}

char Scanner::advance() {
  ++current;
  return source.at(current - 1);
}

std::string_view Scanner::advance(size_t n) {
  current += n;
  return std::string_view(source.data() + current - n, n);
}

bool Scanner::match(char expected) {
  if (isAtEnd()) {
    return false;
  }
  if (source.at(current) != expected) {
    return false;
  }
  ++current;
  return true;
}

char Scanner::peek() const {
  if (isAtEnd()) {
    return '\0';
  }
  return source.at(current);
}

std::string_view Scanner::peekUtf8() const {
  if (isAtEnd()) {
    return {};
  }
  unsigned char firstByte = source[current];
  if (firstByte < static_cast<unsigned char>(0x80)) {
    if (current >= source.size()) {
      return {};
    }
    return std::string_view(source.data() + current, 1);
  } else if ((firstByte & static_cast<unsigned char>(0xE0)) ==
             static_cast<unsigned char>(0xC0)) {
    if (current + 1 >= source.size()) {
      return {};
    }
    return std::string_view(source.data() + current, 2);
  } else if ((firstByte & static_cast<unsigned char>(0xF0)) ==
             static_cast<unsigned char>(0xE0)) {
    if (current + 2 >= source.size()) {
      return {};
    }
    return std::string_view(source.data() + current, 3);
  } else if ((firstByte & static_cast<unsigned char>(0xF8)) ==
             static_cast<unsigned char>(0xF0)) {
    if (current + 3 >= source.size()) {
      return {};
    }
    return std::string_view(source.data() + current, 4);
  } else {
    return std::string_view{};
  }
}

char Scanner::peekNext() const {
  if (current + 1 >= static_cast<int>(source.size())) {
    return '\0';
  }
  return source.at(current + 1);
}

void Scanner::addToken(TokenType type, std::any literal) {
  tokens.emplace_back(type, source.substr(start, current - start), literal,
                      line);
}

void Scanner::addToken(TokenType type) {
  tokens.emplace_back(type, source.substr(start, current - start), line);
}

void Scanner::comment() {
  while (peek() != '\n' && !isAtEnd()) {
    advance();
  }
}

void Scanner::comment_slash_star() {
  while (!isAtEnd() && !(peek() == '*' && peekNext() == '/')) {
    advance();
  }
  // 此时当前字符和下一个字符为*/, skip
  advance();
  advance();
}

void Scanner::string() {
  std::string_view u8str;
  while (true) {
    u8str = peekUtf8();
    if (isAtEnd() || u8str.empty() ||
        (u8str.size() == 1 && u8str.front() == '"')) {
      break;
    }
    if (u8str.size() == 1 && u8str.front() == '\n') {
      ++line;
    }
    advance(u8str.size());
  }

  if (isAtEnd() || u8str.empty()) {
    Lox::Error(line, "Unterminated string.");
  }
  advance(); // the closing '''

  // literal
  addToken(TokenType::String, source.substr(start + 1, current - start - 2));
}

void Scanner::number() {
  while (std::isdigit(peek()) != 0) {
    advance();
  }

  // Look for a fractional part.
  if (peek() == '.' && std::isdigit(peekNext()) != 0) {
    // Consume the "."
    advance();

    while (std::isdigit(peek()) != 0) {
      advance();
    }
  }

  addToken(TokenType::Number, std::stod(source.substr(start, current - start)));
}

void Scanner::identifier() {
  while ((std::isdigit(peek()) != 0) || (std::isalpha(peek()) != 0) ||
         peek() == '_') {
    advance();
  }

  const auto text = source.substr(start, current - start);
  if (const auto iter = keywords.find(text); iter != keywords.end()) {
    addToken(iter->second);
  } else {
    addToken(TokenType::Identifier);
  }
}

void Scanner::scanToken() {
  // 相对于current的前一个字符
  auto prevChar = advance();
  switch (prevChar) {
  case '(':
    addToken(TokenType::LeftParen);
    break;
  case ')':
    addToken(TokenType::RightParen);
    break;
  case '{':
    addToken(TokenType::LeftBrace);
    break;
  case '}':
    addToken(TokenType::RightBrace);
    break;
  case ',':
    addToken(TokenType::Comma);
    break;
  case '.':
    addToken(TokenType::Dot);
    break;
  case '-':
    addToken(TokenType::Minus);
    break;
  case '+':
    addToken(TokenType::Plus);
    break;
  case ';':
    addToken(TokenType::Semicolon);
    break;
  case '*':
    addToken(TokenType::Star);
    break;
  case '!':
    addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
    break;
  case '=':
    addToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
    break;
  case '<':
    addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
    break;
  case '>':
    addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
    break;
  case '/':
    if (match('/')) {
      comment();
    } else if (match('*')) {
      comment_slash_star();
    } else {
      addToken(TokenType::Slash);
    }
    break;
  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace.
    break;
  case '\n':
    line++;
    break;
  case '"':
    string();
    break;

  default:
    if (std::isdigit(prevChar) != 0) {
      number();
    } else if (std::isalpha(prevChar) != 0 || prevChar == '_') {
      identifier();
    } else {
      Lox::Error(line, fmt::format("Unexpected character: '{}'", prevChar));
    }
  }
}

}; // namespace Lox