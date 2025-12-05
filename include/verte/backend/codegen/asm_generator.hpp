/**
 * @brief Assembly generator for NASM output.
 * @file asm_generator.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_ASM_GENERATOR_HPP
#define VERTE_BACKEND_CODEGEN_ASM_GENERATOR_HPP

#include "verte/backend/codegen/instruction_selector.hpp"
#include "verte/backend/ir/module.hpp"
#include "verte/utils/logger.hpp"

#include <sstream>
#include <string>

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  /**
   * @class AsmGenerator
   * @brief Generates NASM assembly output from selected instructions.
   */
  class AsmGenerator {
  public:
    /**
     * @brief Construct a new AsmGenerator.
     */
    AsmGenerator() noexcept : logger("asm-generator") {}

    /**
     * @brief Generate NASM assembly for the module.
     * @param selector The instruction selector with selected instructions.
     * @param module The IR module for global data.
     * @return The generated assembly as a string.
     */
    [[nodiscard]] std::string generate(const InstructionSelector &selector,
                                       const ir::Module &module);

    /**
     * @brief Write assembly to a file.
     * @param selector The instruction selector with selected instructions.
     * @param module The IR module for global data.
     * @param outputPath The output file path.
     * @return True if successful, otherwise false.
     */
    bool writeToFile(const InstructionSelector &selector,
                     const ir::Module &module, const std::string &outputPath);

  private:
    /**
     * @brief Emit the file header and directives.
     * @param oss Output stream.
     * @param selector The instruction selector.
     */
    void emitHeader(std::ostringstream &oss,
                    const InstructionSelector &selector);

    /**
     * @brief Emit external function declarations.
     * @param oss Output stream.
     * @param selector The instruction selector.
     * @param module The IR module to check for defined functions.
     */
    void emitExternDecls(std::ostringstream &oss,
                         const InstructionSelector &selector,
                         const ir::Module &module);

    /**
     * @brief Emit the data section.
     * @param oss Output stream.
     * @param selector The instruction selector.
     */
    void emitDataSection(std::ostringstream &oss,
                         const InstructionSelector &selector);

    /**
     * @brief Emit the BSS section for uninitialized data.
     * @param oss Output stream.
     */
    void emitBssSection(std::ostringstream &oss);

    /**
     * @brief Emit the text section with all functions.
     * @param oss Output stream.
     * @param selector The instruction selector.
     */
    void emitTextSection(std::ostringstream &oss,
                         const InstructionSelector &selector);

    /**
     * @brief Emit a single function.
     * @param oss Output stream.
     * @param func The function code.
     */
    void emitFunction(std::ostringstream &oss, const FunctionCode &func);

    /**
     * @brief Escape a string for NASM.
     * @param str The string to escape.
     * @return The escaped string.
     */
    [[nodiscard]] static std::string escapeString(const std::string &str);

    /**
     * @brief Format a double value for NASM.
     * @param value The double value.
     * @return The formatted string.
     */
    [[nodiscard]] static std::string formatDouble(double value);

    utils::Logger logger; /**< The logger. */
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_ASM_GENERATOR_HPP
