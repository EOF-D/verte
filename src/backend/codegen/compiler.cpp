/**
 * @brief Compiler implementation.
 * @file compiler.cpp
 */

#include "verte/backend/codegen/compiler.hpp"
#include "verte/backend/codegen/asm_generator.hpp"

#include <filesystem>

namespace verte::codegen {
  bool Compiler::compile(Codegen &codegen, const std::string &outputPath) {
    ir::Module &module = codegen.getModule();
    logger.info("Compiling module: {} to {}", module.getName(), outputPath);

    // Determine intermediate file paths.
    std::filesystem::path outPath(outputPath);
    std::string baseName = outPath.stem().string();
    std::filesystem::path directory = outPath.parent_path();
    if (directory.empty()) {
      directory = ".";
    }

    std::string asmPath = (directory / (baseName + ".asm")).string();
    std::string objPath = (directory / (baseName + ".o")).string();

    // Compile to assembly.
    if (!compileToAsm(codegen, asmPath)) {
      return false;
    }

    // Assemble to object file.
    if (!runAssembler(asmPath, objPath)) {
      // Clean up assembly file on error.
      if (!options.emitAssembly) {
        std::filesystem::remove(asmPath);
      }

      return false;
    }

    // Link to executable.
    if (!runLinker(objPath, outputPath)) {
      // Clean up intermediate files on error.
      if (!options.emitAssembly) {
        std::filesystem::remove(asmPath);
      }

      if (!options.emitObject) {
        std::filesystem::remove(objPath);
      }

      return false;
    }

    // Clean up intermediate files if not requested.
    if (!options.emitAssembly) {
      std::filesystem::remove(asmPath);
    }

    if (!options.emitObject) {
      std::filesystem::remove(objPath);
    }

    logger.info("Compiled to: {}", outputPath);
    return true;
  }

  bool Compiler::compileToAsm(Codegen &codegen, const std::string &outputPath) {
    ir::Module &module = codegen.getModule();
    logger.debug("Generating assembly: {}", outputPath);

    // Register allocation.
    auto allocators = runRegisterAllocation(module);

    // Instruction selection.
    InstructionSelector selector =
        runInstructionSelection(module, allocators, codegen);

    // Assembly generation.
    AsmGenerator generator;
    if (!generator.writeToFile(selector, module, outputPath)) {
      logger.error("Failed to write assembly file: " + outputPath);
      return false;
    }

    return true;
  }

  std::unordered_map<std::string, RegisterAllocator>
  Compiler::runRegisterAllocation(ir::Module &module) {
    logger.debug("Running register allocation");

    std::unordered_map<std::string, RegisterAllocator> allocators;
    for (auto &func : module.getFunctions()) {
      auto [iter, _] = allocators.emplace(func->getName(), RegisterAllocator());
      iter->second.allocate(*func);
    }

    return allocators;
  }

  InstructionSelector Compiler::runInstructionSelection(
      const ir::Module &module,
      const std::unordered_map<std::string, RegisterAllocator> &allocators,
      const Codegen &codegen) {
    logger.debug("Running instruction selection");

    InstructionSelector selector;
    selector.select(module, allocators, codegen);

    return selector;
  }

  bool Compiler::runAssembler(const std::string &asmPath,
                              const std::string &objPath) {
    logger.debug("Assembling: {} to {}", asmPath, objPath);

    std::string command =
        options.nasmPath + " -f elf64 -o " + objPath + " " + asmPath;

    int result = executeCommand(command);
    if (result != 0) {
      logger.error("Assembler failed with exit code: " +
                   std::to_string(result));
      return false;
    }

    return true;
  }

  bool Compiler::runLinker(const std::string &objPath,
                           const std::string &outputPath) {
    logger.debug("Linking: {} to {}", objPath, outputPath);

    std::string command = options.gccPath + " -o " + outputPath + " " +
                          objPath + " -no-pie -z noexecstack";

    int result = executeCommand(command);
    if (result != 0) {
      logger.error("Linker failed with exit code: " + std::to_string(result));
      return false;
    }

    return true;
  }

  int Compiler::executeCommand(const std::string &command) {
    return std::system((command + " 2>&1").c_str());
  }
} // namespace verte::codegen
