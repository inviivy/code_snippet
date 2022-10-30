#include <fmt/core.h>
#include <iostream>
#include <string>
#include <string_view>

#include "Scanner.hpp"


void runPrompt() {
  fmt::print("welcome to lox\n");
  std::string code;
  while (true) {
    fmt::print("> ");
    if (std::getline(std::cin, code)) {
      // run code
      Lox::Scanner scan(code);
      auto tokens = scan.scanTokens();
      for (const auto& token : tokens) {
        fmt::print("{}\n", token.toString());
      }
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