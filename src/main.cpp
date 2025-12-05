#include "verte/backend/codegen/codegen.hpp"
#include "verte/backend/codegen/compiler.hpp"

#include "verte/frontend/lexer/lexer.hpp"
#include "verte/frontend/parser/parser.hpp"
#include "verte/frontend/visitors/ast_printer.hpp"

#include "verte/utils/argparser.hpp"
#include "verte/utils/logger.hpp"

#include <string>

using namespace verte;
using namespace verte::codegen;
using namespace verte::visitors;

int main(int argc, char **argv) {
  const utils::Logger logger("main");
  const utils::ArgParser args(argc, argv);
  utils::logging::setLevel(args.getLogLevel());

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
    ASTPrinter printer;
    ast->accept(printer);

    return 0;
  }

  // Generate IR code.
  Codegen codegen("main");
  ast->accept(codegen);

  // Print the IR if requested.
  if (args.shouldPrintIr()) {
    // TODO: Implement IR printing.
    return 0;
  }

  CompilerOptions options{
      .emitAssembly = args.shouldKeepAsm(),
      .emitObject = args.shouldKeepObj(),
  };

  // Compile the module to native code.
  Compiler compiler(options);
  if (!compiler.compile(codegen, outputFile)) {
    logger.error("Failed to compile the module to native code.");
    return -1;
  }

  return 0;
}
