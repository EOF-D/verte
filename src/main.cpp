#include "argparser.hpp"
#include "logger.hpp"

int main(int argc, char *argv[]) {
  excerpt::utils::Logger logger("main");
  excerpt::utils::ArgParser parser(argc, argv);

  if (!parser.getInput().empty()) {
    for (const auto &line : parser.readInput()) {
      llvm::outs() << line << "\n";
    }
  }

  return 0;
}
