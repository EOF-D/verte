#include "verte/frontend/lexer/lexer.hpp"
#include "verte/frontend/parser/parser.hpp"
#include "verte/frontend/visitors/pretty.hpp"
#include "verte/utils/logger.hpp"
#include <string>

using namespace verte;

int main() {
  utils::logging::setLevel(utils::LogLevel::ERROR);

  std::string input;
  std::string source;

  do {
    std::cout << "> ";
    std::getline(std::cin, input);

    if (input == "debug()")
      break;

    source += input;
  } while (input != "debug()");

  lexer::Lexer lexer(source);
  auto tokens = lexer.allTokens();

  nodes::Parser parser(tokens);
  auto ast = parser.parse();

  visitors::PrettyPrinter printer;
  ast->accept(printer);
  return 0;
}
