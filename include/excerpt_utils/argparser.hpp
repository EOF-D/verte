/**
 * @file argparser.hpp
 * @brief Command line argument parser utility.
 */

#ifndef EXCERPT_ARGPARSER_HPP
#define EXCERPT_ARGPARSER_HPP

#ifndef EXCERPT_VERSION
#define EXCERPT_VERSION "v0.1.0"
#endif

#include "logger.hpp"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"

namespace excerpt::utils {
  /**
   * @brief Command line argument parser
   */
  class ArgParser {
  public:
    /**
     * @brief Parse command line arguments
     * @param argc Number of arguments
     * @param argv Argument list
     */
    ArgParser(int argc, char **argv) {
      llvm::cl::HideUnrelatedOptions(category);
      llvm::cl::SetVersionPrinter(printVersion);
      llvm::cl::ParseCommandLineOptions(argc, argv, "Excerpt\n");
    }

    ~ArgParser() {
      llvm::cl::ResetAllOptionOccurrences();
      _input.reset();
      _output.reset();
    }

    /**
     * @brief Get the input filename.
     */
    std::string getInput() const { return _input; }

    /**
     * @brief Get the output filename.
     */
    const std::string getOutput() const { return _output; }

    /**
     * @brief Get version information.
     */
    static void printVersion(llvm::raw_ostream &out) {
      out << "Excerpt " << EXCERPT_VERSION << "\n";
    }

    /**
     * @brief Read the input file.
     * @return The content of the input file.
     */
    llvm::SmallVector<llvm::StringRef> readInput() {
      llvm::SmallVector<llvm::StringRef> lines;

      // Open the input file.
      std::unique_ptr<llvm::MemoryBuffer> buffer;
      auto bufferOrError = llvm::MemoryBuffer::getFile(getInput());
      logger.info("Reading input file: `{}`", getInput());

      if (!bufferOrError) {
        logger.error("Failed to read input file");
        llvm::errs() << bufferOrError.getError().message() << "\n";

        return lines;
      }

      // Read the content of the input file by lines.
      buffer = std::move(bufferOrError.get());
      llvm::StringRef content = buffer->getBuffer();
      content.split(lines, '\n');

      return lines;
    }

  private:
    Logger logger{"argparser"};

    // clang-format off
    llvm::cl::opt<std::string> _input{
      llvm::cl::Positional,
      llvm::cl::desc("<input file>"),
      llvm::cl::Required,
      llvm::cl::ValueRequired,
      llvm::cl::cat(category)
    };

    llvm::cl::opt<std::string> _output{
      "o",
      llvm::cl::desc("Output file"),
      llvm::cl::value_desc("filename"),
      llvm::cl::cat(category)
    };

    llvm::cl::OptionCategory category{"Options to controll the excerpt compiler."};
    // clang-format on
  };
}; // namespace excerpt::utils

#endif // EXCERPT_ARGPARSER_HPP
