/**
 * @brief Handles the compilation of a llvm module.
 * @file compiler.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_COMPILER_HPP
#define VERTE_BACKEND_CODEGEN_COMPILER_HPP

#include "llvm/IR/Module.h"

#include <memory>
#include <string>

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  using namespace llvm;

  /**
   * @brief Compiler class that handles JIT and native compilation for
   * llvm::Module.
   */
  class Compiler {
  public:
    /**
     * @brief Construct a new Compiler object.
     */
    Compiler() noexcept;

    /**
     * @brief Compile the given module into native code.
     * @param module The module to compile.
     * @param outputPath The file path to save the compiled native code.
     * @return True if compilation succeeded, false otherwise.
     */
    bool compile(Module &module, const std::string &outputPath);

  private:
    /**
     * @brief Compile the given module into native code.
     * @param module The module to compile.
     * @param outputPath The file path to save the compiled native code.
     * @return True if compilation succeeded, false otherwise.
     */
    bool native(Module &module, const std::string &outputPath);
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_COMPILER_HPP
