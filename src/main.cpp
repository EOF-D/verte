#include "verte/backend/codegen/codegen.hpp"
#include "verte/backend/codegen/compiler.hpp"

#include "verte/frontend/lexer/lexer.hpp"
#include "verte/frontend/parser/parser.hpp"
#include "verte/frontend/visitors/pretty.hpp"

#include "verte/utils/argparser.hpp"
#include "verte/utils/logger.hpp"

#include <string>

using namespace verte;

int main(int argc, char **argv) {
  utils::logging::setLevel(utils::LogLevel::ERROR);
  utils::Logger logger("main");

  // Parse command line arguments.
  utils::ArgParser args(argc, argv);

  std::string inputFile = args.getInputFile();
  std::string outputFile =
      args.getOutputFile().empty() ? "a.out" : args.getOutputFile();

  // Read the source code from the input file.
  auto sourceOrEmpty = args.readInputFile();
  if (!sourceOrEmpty) {
    logger.error("Failed to read the input file.");
    return -1;
  }

  std::string source = sourceOrEmpty.value();

  lexer::Lexer lexer(source);
  nodes::Parser parser(lexer.allTokens());
  std::unique_ptr<nodes::ProgramNode> ast = parser.parse();

  logger.info("==========START-AST=========");
  if (utils::logging::getLevel() >= utils::LogLevel::INFO) {
    visitors::PrettyPrinter printer;
    ast->accept(printer);
  }
  logger.info("==========END-AST===========");

  llvm::LLVMContext context;
  auto module = std::make_unique<llvm::Module>("<main>", context);
  codegen::Codegen codegen(context, std::move(module));

  logger.info("===========START-IR=========");
  ast->accept(codegen);
  if (utils::logging::getLevel() >= utils::LogLevel::INFO)
    llvm::outs() << codegen.getModule();
  logger.info("===========END-IR===========");

  // Compile the module to native code.
  codegen::Compiler compiler;
  if (!compiler.compile(codegen.getModule(), outputFile)) {
    logger.error("Failed to compile the module to native code.");
    return -1;
  }

  return 0;
}
