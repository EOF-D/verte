#include "excerpt/lexer/lexer.hpp"
#include "excerpt_utils/logger.hpp"

#include <iostream>

using namespace excerpt;

int main(int argc, char *argv[]) {
  utils::Logger logger("main");
  std::string input;

  do {
    std::cout << "> ";
    std::getline(std::cin, input);

    Lexer lexer(input);
    Token token = lexer.next();

    while (!token.is(Token::Type::END)) {
      logger.info("Token: {}", token.toString());
      token = lexer.next();
    }

    std::cout << "\n";
  } while (input != "exit");

  return 0;
}
