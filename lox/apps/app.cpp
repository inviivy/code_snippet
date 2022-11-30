#include "Interpreter.hpp"
#include "Parser.hpp"
#include "RPNVisitor.hpp"
#include "Scanner.hpp"
#include "TinyVisitor.hpp"

#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

void runPrompt() {
  fmt::print("welcome to lox\n");
  std::string code;
  while (true) {
    fmt::print("> ");
    if (std::getline(std::cin, code)) {
      // run code
      Lox::Scanner scan(code);
      auto tokens = scan.scanTokens();
      Lox::Parser parser(tokens);
      auto exprs = parser.parse();
      auto interpret = std::make_unique<Lox::Interpreter>();
      interpret->interpret(exprs);
    } else {
      fmt::print("\n");
      break;
    }
  }
}

int main(int args, const char *argv[]) {
  if (args > 2) {
    fmt::print("usage: lox [script]\n");
    exit(1);
  } else if (args == 2) {
    // from file
  } else {
    runPrompt();
  }
}