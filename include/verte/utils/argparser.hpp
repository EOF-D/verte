/**
 * @brief Argument parser for command line arguments.
 * @file argparser.hpp
 */

#ifndef VERTE_UTILS_ARGPARSER_HPP
#define VERTE_UTILS_ARGPARSER_HPP

/**
 * @def VERTE_VERSION
 * @brief The version of the compiler.
 */
#ifndef VERTE_VERSION
#  define VERTE_VERSION "0.1.0"
#endif // VERTE_VERSION

#include "verte/utils/logger.hpp"
#include "llvm/Support/CommandLine.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

/**
 * @namespace verte::utils
 * @brief The namespace for utility functions.
 */
namespace verte::utils {
  /**
   * @class ArgParser
   * @brief The argument parser for command line arguments.
   */
  class ArgParser {
  public:
    /**
     * @brief Construct a new ArgParser.
     * @param argc Number of arguments.
     * @param argv Argument list.
     */
    explicit ArgParser(int argc, char **argv) : logger("argparser") {
      llvm::cl::HideUnrelatedOptions(category);
      llvm::cl::SetVersionPrinter([](llvm::raw_ostream &out) {
        out << "Verte v" << VERTE_VERSION << "\n";
      });

      llvm::cl::ParseCommandLineOptions(argc, argv, "Vertec\n");
      logger.info("Initialized argument parser.");
    }

    /**
     * @brief Get the input file.
     * @return The input file.
     */
    [[nodiscard]] std::filesystem::path getInputFile() const {
      return std::filesystem::path(inputFile.getValue());
    }

    /**
     * @brief Get the output file.
     * @return The output file.
     */
    [[nodiscard]] std::filesystem::path getOutputFile() const {
      return std::filesystem::path(outputFile.getValue());
    }

    /**
     * @brief Read the input file.
     * @return The content of the input file.
     */
    std::optional<std::string> readInputFile() const {
      logger.info("Reading input file: {}", getInputFile().string());

      const auto filePath = getInputFile();
      std::ifstream file(filePath, std::ios::binary);
      if (!file) {
        logger.error("Error opening file: {}", filePath.string());
        std::cerr << "Error opening file: " << filePath << "\n";
        return std::nullopt;
      }

      // clang-format off
      std::string contents(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

      // clang-format on
      return contents;
    }

  private:
    using StringOption = llvm::cl::opt<std::string>;

    /**
     * @brief Input option.
     */
    // clang-format off
    StringOption inputFile{
        llvm::cl::Positional,
        llvm::cl::desc("<input file>"),
        llvm::cl::Required,
        llvm::cl::ValueRequired,
        llvm::cl::cat(category)}; /**< The input file. */
            
    /**
     * @brief Output option.
     */
    StringOption outputFile{
        "o",
        llvm::cl::desc("Output file"),
        llvm::cl::value_desc("filename"),
        llvm::cl::cat(category)};

    /**
     * @brief Category for the options.
     */
    llvm::cl::OptionCategory category{"Options to control the excerpt compiler."};

    Logger logger; /**< The logger for the ArgParser. */
    // clang-format on
  };
} // namespace verte::utils

#endif // VERTE_UTILS_ARGPARSER_HPP