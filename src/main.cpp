#include "excerpt/lexer/token.hpp"
#include <iostream>

using namespace excerpt;

int main() {
  llvm::StringRef source = "std::cout << \"Hello, world!\" << std::endl;";
  return 0;
}
