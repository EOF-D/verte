/**
 * @brief Compiler implementation.
 * @file compiler.cpp
 */

#include "verte/backend/codegen/compiler.hpp"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

namespace verte::codegen {
  Compiler::Compiler() noexcept {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
  }

  bool Compiler::compile(Module &module, const std::string &outputPath) {
    if (!native(module, outputPath))
      return false;

    std::string command = "gcc " + outputPath + ".o" + " -o " + outputPath;
    int result = std::system(command.c_str());

    if (result != 0) {
      errs() << "Error: Linking failed: " << result << "\n";
      return false;
    }

    // Clean up the temporary object file.
    std::remove((outputPath + ".o").c_str());
    return true;
  }

  bool Compiler::native(Module &module, const std::string &outputPath) {
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    std::string error;

    auto target = TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
      errs() << error;
      return false;
    }

    auto cpu = "generic";
    auto features = "";

    TargetOptions options;
    auto targetMachine = target->createTargetMachine(
        targetTriple, cpu, features, options, Reloc::PIC_);

    module.setDataLayout(targetMachine->createDataLayout());
    module.setTargetTriple(targetTriple);

    std::error_code errorCode;
    raw_fd_ostream dest(outputPath + ".o", errorCode,
                        sys::fs::OpenFlags::OF_None);

    if (errorCode) {
      errs() << errorCode.message();
      return false;
    }

    legacy::PassManager pass;
    auto fileType = CodeGenFileType::CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
      errs() << "targetMachine can't emit a file of this type";
      return false;
    }

    pass.run(module);
    dest.flush();

    return true;
  }
} // namespace verte::codegen
