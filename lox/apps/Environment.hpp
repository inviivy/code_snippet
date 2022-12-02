#pragma once

#include "Token.hpp"

#include <any>
#include <string>
#include <unordered_map>

namespace Lox {
class Environment {
  std::unordered_map<std::string, std::any>
      values; /* kv pair: variable name : expr or specific value */

public:
  Environment();
  ~Environment();
  void define(const std::string &, const std::any &); /* 定义 */
  void assign(const Token &, const std::any &);       /* 赋值 */
  const std::any &getVal(const Token &) const;
};
}; // namespace Lox