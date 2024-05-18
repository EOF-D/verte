#include "excerpt/lexer/lexer.hpp"
#include "excerpt/parser/parser.hpp"
#include <iostream>

using namespace excerpt;

int main() {
  utils::Logger logger("main");

  std::string input;
  std::string source;

  do {
    std::cout << "> ";
    std::getline(std::cin, input);

    if (input == "debug()")
      break;

    source += input + "\n";
  } while (input != "debug()");

  Lexer lexer(source);
  auto tokens = lexer.all();

  Parser parser(tokens);
  std::unique_ptr<ProgramAST> ast;

  try {
    ast = parser.parse();
  } catch (const ParserError &e) {
    return -1;
  }

  // Print the AST
  for (const auto &expr : ast->body) {
    logger.info("{}", expr->str());
  }

  return 0;
}
