/**
 * @brief Assembly generator implementation.
 * @file asm_generator.cpp
 */

#include "verte/backend/codegen/asm_generator.hpp"

#include <cmath>
#include <fstream>

namespace verte::codegen {
  std::string AsmGenerator::generate(const InstructionSelector &selector,
                                     const ir::Module &module) {
    logger.debug("Generating assembly for module: {}", module.getName());
    std::ostringstream oss;

    // Emit file structure.
    emitHeader(oss, selector);
    emitExternDecls(oss, selector, module);
    emitDataSection(oss, selector);
    emitBssSection(oss);
    emitTextSection(oss, selector);

    logger.debug("Assembly generation complete");
    logger.debug("ASM Output:\n{}", oss.str());
    return oss.str();
  }

  bool AsmGenerator::writeToFile(const InstructionSelector &selector,
                                 const ir::Module &module,
                                 const std::string &outputPath) {
    logger.info("Writing assembly to: {}", outputPath);
    std::string assembly = generate(selector, module);

    std::ofstream file(outputPath);
    if (!file) {
      logger.error("Failed to open file: {}", outputPath);
      return false;
    }

    file << assembly;
    file.close();

    if (!file) {
      logger.error("Failed to write to file: {}", outputPath);
      return false;
    }

    logger.info("Wrote {} bytes to {}", assembly.size(), outputPath);
    return true;
  }

  void AsmGenerator::emitHeader(std::ostringstream &oss,
                                const InstructionSelector &selector) {
    oss << "; Verte Compiler\n";
    oss << "; Target: x86_64 Linux\n";

    // Add more directives if needed later.

    oss << "\n";
  }

  void AsmGenerator::emitExternDecls(std::ostringstream &oss,
                                     const InstructionSelector &selector,
                                     const ir::Module &module) {
    const auto &externFuncs = selector.getExternFunctions();
    if (externFuncs.empty()) {
      return;
    }

    for (const auto &func : externFuncs) {
      if (!module.hasFunction(func)) {
        oss << "extern " << func << "\n";
      }
    }

    oss << "\n";
  }

  void AsmGenerator::emitDataSection(std::ostringstream &oss,
                                     const InstructionSelector &selector) {
    const auto &floatConsts = selector.getFloatConstants();
    const auto &strConsts = selector.getStringConstants();
    if (floatConsts.empty() && strConsts.empty()) {
      return;
    }

    oss << "section .data\n";

    // Emit floating-point constants.
    for (const auto &fc : floatConsts) {
      oss << "    " << fc.label << ": dq " << formatDouble(fc.value) << "\n";
    }

    // Emit string constants.
    for (const auto &sc : strConsts) {
      oss << "    " << sc.label << ": db " << escapeString(sc.value) << ", 0\n";
    }

    oss << "\n";
  }

  void AsmGenerator::emitBssSection(std::ostringstream &oss) {
    // Currently not needed as we only support constants for globals.
  }

  void AsmGenerator::emitTextSection(std::ostringstream &oss,
                                     const InstructionSelector &selector) {
    oss << "section .text\n";

    // Export all functions as global.
    for (const auto &func : selector.getFunctions()) {
      if (func.name == "printf") {
        continue;
      }

      oss << "    global " << func.name << "\n";
    }

    oss << "\n";

    // Emit each function.
    for (const auto &func : selector.getFunctions()) {
      emitFunction(oss, func);
    }
  }

  void AsmGenerator::emitFunction(std::ostringstream &oss,
                                  const FunctionCode &func) {
    if (func.name == "printf") {
      return;
    }

    // Function label.
    oss << func.name << ":\n";

    // Emit prologue.
    for (const auto &instr : func.prologue) {
      oss << instr.toString() << "\n";
    }

    // Emit body.
    for (const auto &instr : func.body) {
      oss << instr.toString() << "\n";
    }

    // Emit epilogue.
    for (const auto &instr : func.epilogue) {
      oss << instr.toString() << "\n";
    }

    oss << "\n";
  }

  std::string AsmGenerator::escapeString(const std::string &str) {
    std::ostringstream oss;
    oss << "\"";

    for (char c : str) {
      switch (c) {
        case '\n':
          oss << "\", 10, \"";
          break;
        case '\r':
          oss << "\", 13, \"";
          break;
        case '\t':
          oss << "\", 9, \"";
          break;
        case '\\':
          oss << "\\\\";
          break;
        case '"':
          oss << "\\\"";
          break;
        default:
          if (c >= 32 && c < 127) {
            oss << c;
          }

          // Non-printable characters are emitted as bytes.
          else {
            oss << "\", " << static_cast<int>(static_cast<unsigned char>(c))
                << ", \"";
          }

          break;
      }
    }

    oss << "\"";

    // Clean up empty strings from escaping.
    std::string result = oss.str();

    // Remove empty string literals like "", or , ""
    std::string cleaned;
    size_t i = 0;
    while (i < result.size()) {
      if (i == 0 && result.substr(0, 4) == "\"\", ") {
        i += 4;
        continue;
      }

      if (i + 4 == result.size() && result.substr(i, 4) == ", \"\"") {
        break;
      }

      if (i + 6 <= result.size() && result.substr(i, 6) == ", \"\", ") {
        i += 6;
        continue;
      }

      cleaned += result[i];
      i++;
    }

    return cleaned.empty() ? "\"\"" : cleaned;
  }

  std::string AsmGenerator::formatDouble(double value) {
    // Handle special values.
    if (std::isnan(value)) {
      return "0x7FF8000000000000"; // Quiet NaN.
    }

    if (std::isinf(value)) {
      if (value > 0) {
        return "0x7FF0000000000000"; // +Infinity.
      }

      else {
        return "0xFFF0000000000000"; // -Infinity.
      }
    }

    if (value == 0.0 && std::signbit(value)) {
      return "0x8000000000000000"; // -0.0.
    }

    // Regular floating-point value.
    union {
      double value;
      uint64_t i;
    } bits;

    bits.value = value;

    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(16) << bits.i;

    return oss.str();
  }
} // namespace verte::codegen
