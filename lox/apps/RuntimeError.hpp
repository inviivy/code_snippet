#pragma once

#include <stdexcept>

#include "Token.hpp"

namespace Lox {
class RuntimeError : public std::runtime_error {
  Token m_token;

public:
  RuntimeError(const Token &token, const std::string &message)
      : std::runtime_error(message), m_token(token) {}

  const Token &getToken() const { return m_token; }
};
}; // namespace Lox