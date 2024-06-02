#include "excerpt/lexer/lexer.hpp"
#include "excerpt/parser/parser.hpp"
#include "excerpt/visitors/pretty.hpp"

#include <iostream>

using namespace excerpt;

int main() {
  utils::Logging::setLevel(utils::LogLevel::ERROR);

  std::string input;
  std::string source;

  do {
    std::cout << "> ";
    std::getline(std::cin, input);

    if (input == "debug()")
      break;

    source += input;
  } while (input != "debug()");

  Lexer lexer(source);
  auto tokens = lexer.all();

  Parser parser(tokens);
  auto ast = parser.parse();

  PrettyPrinter printer;
  ast->accept(printer);
  return 0;
}
