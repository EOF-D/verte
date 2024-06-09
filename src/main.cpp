#include "verte/backend/codegen/codegen.hpp"
#include "verte/backend/codegen/compiler.hpp"

#include "verte/frontend/lexer/lexer.hpp"
#include "verte/frontend/parser/parser.hpp"
#include "verte/frontend/visitors/pretty.hpp"

#include "verte/utils/argparser.hpp"
#include "verte/utils/logger.hpp"

#include <string>

using namespace verte;
using namespace verte::codegen;
using namespace verte::visitors;

int main(int argc, char **argv) {
  utils::logging::setLevel(utils::LogLevel::DEBUG);
  const utils::Logger logger("main");
  const utils::ArgParser args(argc, argv);

  const std::string inputFile = args.getInputFile();
  const std::string outputFile =
      args.getOutputFile().empty() ? "a.out" : args.getOutputFile();

  // Read the source code from the input file.
  const auto sourceOrEmpty = args.readInputFile();
  if (!sourceOrEmpty) {
    logger.error("Failed to read the input file.");
    return -1;
  }

  // Lex and parse the source code.
  const std::string source = sourceOrEmpty.value();
  lexer::Lexer lexer(source);
  nodes::Parser parser(lexer.allTokens());

  // Print the AST if requested.
  const auto ast = parser.parse();
  if (args.shouldPrintAst()) {
    PrettyPrinter printer;
    ast->accept(printer);

    return 0;
  }

  // Generate target code.
  llvm::LLVMContext context;
  Codegen codegen(context, std::make_unique<llvm::Module>("main", context));
  ast->accept(codegen);

  // Print the LLVM IR if requested.
  if (args.shouldPrintIr()) {
    codegen.getModule().print(llvm::outs(), nullptr);
    return 0;
  }

  // Compile the module to native code.
  codegen::Compiler compiler;
  if (!compiler.compile(codegen.getModule(), outputFile)) {
    logger.error("Failed to compile the module to native code.");
    return -1;
  }

  return 0;
}
