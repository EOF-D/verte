/**
 * @brief Handles the compilation of an IR module.
 * @file compiler.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_COMPILER_HPP
#define VERTE_BACKEND_CODEGEN_COMPILER_HPP

#include "verte/backend/ir/module.hpp"

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  /**
   * @brief Compiler class that handles native compilation for IR modules.
   */
  class Compiler {
  public:
    /**
     * @brief Construct a new Compiler object.
     */
    Compiler() noexcept = default;

    /**
     * @brief Compile the given IR module into native code.
     * @param module The IR module to compile.
     * @param outputPath The file path to save the compiled native code.
     * @return True if compilation succeeded, false otherwise.
     */
    bool compile(ir::Module &module, const std::string &outputPath);
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_COMPILER_HPP
