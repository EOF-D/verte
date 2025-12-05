/**
 * @brief Handles the compilation of an IR module to native code.
 * @file compiler.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_COMPILER_HPP
#define VERTE_BACKEND_CODEGEN_COMPILER_HPP

#include "verte/backend/codegen/codegen.hpp"
#include "verte/backend/codegen/instruction_selector.hpp"
#include "verte/backend/codegen/register_allocator.hpp"
#include "verte/backend/ir/module.hpp"
#include "verte/utils/logger.hpp"

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  /**
   * @struct CompilerOptions
   * @brief Options for the compiler.
   */
  struct CompilerOptions {
    bool emitAssembly = false;     /**< Keep the generated .asm file. */
    bool emitObject = false;       /**< Keep the generated .o file. */
    std::string nasmPath = "nasm"; /**< Path to NASM assembler. */
    std::string gccPath = "gcc";   /**< Path to C compiler. */
  };

  /**
   * @class Compiler
   * @brief Compiler class that handles native compilation for IR modules.
   */
  class Compiler {
  public:
    /**
     * @brief Construct a new Compiler.
     */
    Compiler() noexcept : logger("compiler") {}

    /**
     * @brief Construct a new Compiler with options.
     * @param options The compiler options.
     */
    explicit Compiler(CompilerOptions options) noexcept
        : options(std::move(options)), logger("compiler") {}

    /**
     * @brief Compile the given IR module into native code.
     * @param codegen The codegen instance.
     * @param outputPath The file path to save the compiled native code.
     * @return True if compilation succeeded, otherwise false.
     */
    bool compile(Codegen &codegen, const std::string &outputPath);

    /**
     * @brief Compile to assembly only.
     * @param codegen The codegen instance.
     * @param outputPath The output .asm file path.
     * @return True if successful, otherwise false.
     */
    bool compileToAsm(Codegen &codegen, const std::string &outputPath);

  private:
    /**
     * @brief Run register allocation for all functions.
     * @param module The IR module.
     * @return Map of function names to allocators.
     */
    std::unordered_map<std::string, RegisterAllocator>
    runRegisterAllocation(ir::Module &module);

    /**
     * @brief Run instruction selection.
     * @param module The IR module.
     * @param allocators The register allocators.
     * @param codegen The codegen instance.
     * @return The instruction selector with selected instructions.
     */
    InstructionSelector runInstructionSelection(
        const ir::Module &module,
        const std::unordered_map<std::string, RegisterAllocator> &allocators,
        const Codegen &codegen);

    /**
     * @brief Run the assembler.
     * @param asmPath Path to the assembly file.
     * @param objPath Path for the output object file.
     * @return True if successful, otherwise false.
     */
    bool runAssembler(const std::string &asmPath, const std::string &objPath);

    /**
     * @brief Run the linker.
     * @param objPath Path to the object file.
     * @param outputPath Path for the output executable.
     * @return True if successful, otherwise false.
     */
    bool runLinker(const std::string &objPath, const std::string &outputPath);

    /**
     * @brief Execute a shell command.
     * @param command The command to execute.
     * @return The exit code.
     */
    int executeCommand(const std::string &command);

    CompilerOptions options; /**< Compiler options. */
    utils::Logger logger;    /**< The logger. */
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_COMPILER_HPP
