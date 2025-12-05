/**
 * @brief Instruction selector implementation.
 * @file instruction_selector.cpp
 */

#include "verte/backend/codegen/instruction_selector.hpp"

#include <cmath>

namespace verte::codegen {
  void InstructionSelector::select(
      const ir::Module &module,
      const std::unordered_map<std::string, RegisterAllocator> &allocators,
      const Codegen &codegen) {
    logger.debug("Selecting instructions for module: {}", module.getName());

    // Clear previous state.
    functions.clear();
    floatConstants.clear();
    stringConstants.clear();
    externFunctions.clear();
    nextFloatLabel = 0;
    nextStringLabel = 0;

    // Extract string constants and external functions from codegen.
    extractStringConstants(codegen);
    extractExternalFunctions(codegen);

    // Select instructions for each function.
    for (const auto &func : module.getFunctions()) {
      auto iter = allocators.find(func->getName());
      if (iter == allocators.end()) {
        logger.error("No register allocator for function: {}", func->getName());
        continue;
      }

      functions.push_back(selectFunction(*func, iter->second));
    }

    logger.debug("Completed selection for module: {}", module.getName());
    logger.debug("Total functions selected: {}", functions.size());
    logger.debug("Total float constants: {}", floatConstants.size());
    logger.debug("Total string constants: {}", stringConstants.size());
  }

  void InstructionSelector::extractStringConstants(const Codegen &codegen) {
    const auto &strings = codegen.getStrings();
    for (const auto &[value, operand] : strings) {
      stringConstants.emplace_back(operand.getGlobalName(), value);
    }

    logger.debug("Extracted {} string constants", stringConstants.size());
  }

  void InstructionSelector::extractExternalFunctions(const Codegen &codegen) {
    const auto &externs = codegen.getExternalFunctions();
    for (const auto &funcName : externs) {
      externFunctions.insert(funcName);
    }

    logger.debug("Extracted {} external functions", externFunctions.size());
  }

  FunctionCode
  InstructionSelector::selectFunction(const ir::Function &func,
                                      const RegisterAllocator &allocator) {
    logger.debug("Selecting for function: {}", func.getName());
    currentFunctionName = func.getName();

    FunctionCode code(func.getName());
    code.usedCalleeSaved = allocator.getUsedCalleeSaved();

    // Generate prologue.
    generatePrologue(func, allocator, code);

    // Select instructions for each block.
    for (const auto &block : func.getBlocks()) {
      if (block.get() != func.getEntryBlock()) {
        code.body.push_back(X86Instruction(makeLabel(block->getLabel()) + ":"));
      }

      selectBlock(*block, allocator, code.body);
    }

    // Generate epilogue.
    generateEpilogue(func, allocator, code);
    return code;
  }

  void InstructionSelector::selectBlock(const ir::BasicBlock &block,
                                        const RegisterAllocator &allocator,
                                        std::vector<X86Instruction> &code) {
    for (const auto &instr : block.getInstructions()) {
      selectInstruction(instr, allocator, code);
    }
  }

  void
  InstructionSelector::selectInstruction(const ir::Instruction &instr,
                                         const RegisterAllocator &allocator,
                                         std::vector<X86Instruction> &code) {
    switch (instr.getOpcode()) {
      using enum ir::Opcode;

      // Integer arithmetic.
      case ADD:
      case SUB:
      case MUL:
      case DIV:
      case MOD:

      // Float arithmetic.
      case FADD:
      case FSUB:
      case FMUL:
      case FDIV:
        selectBinaryOp(instr, allocator, code);
        break;

      // Integer comparisons.
      case ICMP_EQ:
      case ICMP_NE:
      case ICMP_LT:
      case ICMP_LE:
      case ICMP_GT:
      case ICMP_GE:

      // Float comparisons.
      case FCMP_EQ:
      case FCMP_NE:
      case FCMP_LT:
      case FCMP_LE:
      case FCMP_GT:
      case FCMP_GE:
        selectCompareOp(instr, allocator, code);
        break;

      // Unary operations.
      case NEG:
      case FNEG:
      case NOT:
        selectUnaryOp(instr, allocator, code);
        break;

      // Type conversions.
      case ITOF:
      case FTOI:
        selectConversion(instr, allocator, code);
        break;

      // Memory operations.
      case LOAD:
        selectLoad(instr, allocator, code);
        break;

      case STORE:
        selectStore(instr, allocator, code);
        break;

      // Control flow.
      case BR:
        selectBranch(instr, code);
        break;

      case CONDBR:
        selectCondBranch(instr, allocator, code);
        break;

      case CALL:
        selectCall(instr, allocator, code);
        break;

      case RET:
        selectReturn(instr, allocator, code);
        break;
    }
  }

  void InstructionSelector::selectBinaryOp(const ir::Instruction &instr,
                                           const RegisterAllocator &allocator,
                                           std::vector<X86Instruction> &code) {
    const auto &dest = instr.getDest();
    const auto &operands = instr.getOperands();
    const auto &lhs = operands[0];
    const auto &rhs = operands[1];

    std::string destStr = formatOperand(dest, allocator);
    std::string lhsStr = formatOperand(lhs, allocator);
    std::string rhsStr = formatOperand(rhs, allocator);

    switch (instr.getOpcode()) {
      using enum ir::Opcode;

      case ADD:
        code.push_back(X86Instruction("mov", destStr, lhsStr));
        code.push_back(X86Instruction("add", destStr, rhsStr));
        break;

      case SUB:
        code.push_back(X86Instruction("mov", destStr, lhsStr));
        code.push_back(X86Instruction("sub", destStr, rhsStr));
        break;

      case MUL:
        code.push_back(X86Instruction("mov", destStr, lhsStr));
        code.push_back(X86Instruction("imul", destStr, rhsStr));
        break;

      case DIV: {
        code.push_back(X86Instruction("mov", "rax", lhsStr));
        code.push_back(X86Instruction("cqo", "", "", "Sign extend RAX to RDX"));
        code.push_back(X86Instruction("idiv", rhsStr));
        code.push_back(X86Instruction("mov", destStr, "rax"));
        break;
      }

      case MOD: {
        code.push_back(X86Instruction("mov", "rax", lhsStr));
        code.push_back(X86Instruction("cqo"));
        code.push_back(X86Instruction("idiv", rhsStr));
        code.push_back(X86Instruction("mov", destStr, "rdx"));
        break;
      }

      case FADD: {
        code.push_back(X86Instruction("movsd", destStr, lhsStr));
        code.push_back(X86Instruction("addsd", destStr, rhsStr));
        break;
      }

      case FSUB:
        code.push_back(X86Instruction("movsd", destStr, lhsStr));
        code.push_back(X86Instruction("subsd", destStr, rhsStr));
        break;

      case FMUL:
        code.push_back(X86Instruction("movsd", destStr, lhsStr));
        code.push_back(X86Instruction("mulsd", destStr, rhsStr));
        break;

      case FDIV:
        code.push_back(X86Instruction("movsd", destStr, lhsStr));
        code.push_back(X86Instruction("divsd", destStr, rhsStr));
        break;

      default:
        break;
    }
  }

  void InstructionSelector::selectUnaryOp(const ir::Instruction &instr,
                                          const RegisterAllocator &allocator,
                                          std::vector<X86Instruction> &code) {
    const auto &dest = instr.getDest();
    const auto &operands = instr.getOperands();
    const auto &src = operands[0];

    std::string destStr = formatOperand(dest, allocator);
    std::string srcStr = formatOperand(src, allocator);

    switch (instr.getOpcode()) {
      using enum ir::Opcode;

      case NEG:
        code.push_back(X86Instruction("mov", destStr, srcStr));
        code.push_back(X86Instruction("neg", destStr));
        break;

      case FNEG: {
        std::string signMask = createFloatConstant(-0.0);
        code.push_back(X86Instruction("movsd", destStr, srcStr));
        code.push_back(
            X86Instruction("xorpd", destStr, "[rel " + signMask + "]"));

        break;
      }

      case NOT:
        code.push_back(X86Instruction("xor", "eax", "eax"));
        code.push_back(X86Instruction("cmp", srcStr, "0"));
        code.push_back(X86Instruction("sete", "al"));
        code.push_back(X86Instruction("movzx", destStr, "al"));
        break;

      default:
        break;
    }
  }

  void InstructionSelector::selectCompareOp(const ir::Instruction &instr,
                                            const RegisterAllocator &allocator,
                                            std::vector<X86Instruction> &code) {
    const auto &dest = instr.getDest();
    const auto &operands = instr.getOperands();
    const auto &lhs = operands[0];
    const auto &rhs = operands[1];

    std::string destStr = formatOperand(dest, allocator);
    std::string lhsStr = formatOperand(lhs, allocator);
    std::string rhsStr = formatOperand(rhs, allocator);

    bool isFloat = isFloatType(lhs.getTypeInfo());
    if (isFloat) {
      code.push_back(X86Instruction("ucomisd", lhsStr, rhsStr));
    }

    else {
      code.push_back(X86Instruction("cmp", lhsStr, rhsStr));
    }

    // Select the appropriate set instruction.
    std::string setInstr;
    switch (instr.getOpcode()) {
      using enum ir::Opcode;

      case ICMP_EQ:
      case FCMP_EQ:
        setInstr = "sete";
        break;

      case ICMP_NE:
      case FCMP_NE:
        setInstr = "setne";
        break;

      case ICMP_LT:
        setInstr = "setl";
        break;

      case FCMP_LT:
        setInstr = "setb";
        break;

      case ICMP_LE:
        setInstr = "setle";
        break;

      case FCMP_LE:
        setInstr = "setbe";
        break;

      case ICMP_GT:
        setInstr = "setg";
        break;

      case FCMP_GT:
        setInstr = "seta";
        break;

      case ICMP_GE:
        setInstr = "setge";
        break;

      case FCMP_GE:
        setInstr = "setae";
        break;

      default:
        setInstr = "sete";
        break;
    }

    // Set result based on flags.
    code.push_back(X86Instruction(setInstr, "al"));
    code.push_back(X86Instruction("movzx", destStr, "al"));
  }

  void InstructionSelector::selectLoad(const ir::Instruction &instr,
                                       const RegisterAllocator &allocator,
                                       std::vector<X86Instruction> &code) {
    const auto &dest = instr.getDest();
    const auto &src = instr.getOperands()[0];

    std::string destStr = formatOperand(dest, allocator);
    std::string srcStr = formatOperand(src, allocator);

    bool isFloat = isFloatType(dest.getTypeInfo());
    if (isFloat) {
      code.push_back(X86Instruction("movsd", destStr, srcStr));
    }

    else {
      code.push_back(X86Instruction("mov", destStr, srcStr));
    }
  }

  void InstructionSelector::selectStore(const ir::Instruction &instr,
                                        const RegisterAllocator &allocator,
                                        std::vector<X86Instruction> &code) {
    const auto &dest = instr.getDest();
    const auto &src = instr.getOperands()[0];

    std::string destStr = formatOperand(dest, allocator);
    std::string srcStr = formatOperand(src, allocator);

    bool isFloat = isFloatType(src.getTypeInfo());
    if (isFloat) {
      code.push_back(X86Instruction("movsd", destStr, srcStr));
    }

    else {
      code.push_back(X86Instruction("mov", destStr, srcStr));
    }
  }

  void InstructionSelector::selectBranch(const ir::Instruction &instr,
                                         std::vector<X86Instruction> &code) {
    code.push_back(X86Instruction("jmp", makeLabel(instr.getLabel())));
  }

  void
  InstructionSelector::selectCondBranch(const ir::Instruction &instr,
                                        const RegisterAllocator &allocator,
                                        std::vector<X86Instruction> &code) {
    const auto &cond = instr.getOperands()[0];
    std::string condStr = formatOperand(cond, allocator);

    code.push_back(X86Instruction("test", condStr, condStr));
    code.push_back(X86Instruction("jnz", makeLabel(instr.getLabel())));

    if (instr.getFalseLabel().has_value()) {
      code.push_back(X86Instruction("jmp", makeLabel(*instr.getFalseLabel())));
    }
  }

  void InstructionSelector::selectCall(const ir::Instruction &instr,
                                       const RegisterAllocator &allocator,
                                       std::vector<X86Instruction> &code) {
    const auto &args = instr.getOperands();
    const std::string &funcName = instr.getFuncName();
    const auto &dest = instr.getDest();

    // Track argument register indices.
    size_t intArgIndex = 0;
    size_t floatArgIndex = 0;
    size_t stackArgs = 0;

    // Calculate stack space needed for stack arguments.
    for (size_t i = 0; i < args.size(); ++i) {
      bool isFloat = isFloatType(args[i].getTypeInfo());
      if (isFloat) {
        if (floatArgIndex >= 8) {
          stackArgs++;
        }

        floatArgIndex++;
      }

      else {
        if (intArgIndex >= 6) {
          stackArgs++;
        }

        intArgIndex++;
      }
    }

    // Align stack to 16 bytes if needed.
    size_t stackSpace = stackArgs * 8;
    if (stackSpace % 16 != 0) {
      stackSpace += 8;
    }

    if (stackSpace > 0) {
      code.push_back(X86Instruction("sub", "rsp", std::to_string(stackSpace)));
    }

    // Reset indices for actual argument passing.
    intArgIndex = 0;
    floatArgIndex = 0;
    size_t stackOffset = 0;

    // Pass arguments.
    for (size_t i = 0; i < args.size(); ++i) {
      bool isFloat = isFloatType(args[i].getTypeInfo());
      if (isFloat) {
        std::string argStr = formatOperand(args[i], allocator);
        if (floatArgIndex < 8) {
          std::string xmmReg = getRegName64(FLOAT_ARG_REGS[floatArgIndex]);
          code.push_back(X86Instruction("movsd", xmmReg, argStr));
        }

        else {
          std::string stackLoc = "[rsp + " + std::to_string(stackOffset) + "]";
          code.push_back(X86Instruction("movsd", stackLoc, argStr));
          stackOffset += 8;
        }

        floatArgIndex++;
      }

      else {
        if (intArgIndex < 6) {
          std::string intReg = getRegName64(INT_ARG_REGS[intArgIndex]);
          if (args[i].isGlobal()) {
            // For strings, use LEA to load the address
            std::string labelRef = "[rel " + args[i].getGlobalName() + "]";
            code.push_back(X86Instruction("lea", intReg, labelRef));
          }

          else {
            std::string argStr = formatOperand(args[i], allocator);
            code.push_back(X86Instruction("mov", intReg, argStr));
          }
        }

        else {
          std::string argStr = formatOperand(args[i], allocator);
          std::string stackLoc = "[rsp + " + std::to_string(stackOffset) + "]";
          code.push_back(X86Instruction("mov", stackLoc, argStr));
          stackOffset += 8;
        }

        intArgIndex++;
      }
    }

    // Call the function.
    code.push_back(X86Instruction("call", funcName));

    // Clean up stack if we allocated any.
    if (stackSpace > 0) {
      code.push_back(X86Instruction("add", "rsp", std::to_string(stackSpace)));
    }

    // Move return value to destination.
    if (dest.isVReg()) {
      std::string destStr = formatOperand(dest, allocator);
      bool isFloat = isFloatType(dest.getTypeInfo());

      if (isFloat) {
        if (destStr != "xmm0") {
          code.push_back(X86Instruction("movsd", destStr, "xmm0"));
        }
      } else {
        if (destStr != "rax") {
          code.push_back(X86Instruction("mov", destStr, "rax"));
        }
      }
    }
  }

  void InstructionSelector::selectReturn(const ir::Instruction &instr,
                                         const RegisterAllocator &allocator,
                                         std::vector<X86Instruction> &code) {
    const auto &operands = instr.getOperands();

    if (!operands.empty()) {
      const auto &retVal = operands[0];
      std::string retStr = formatOperand(retVal, allocator);

      bool isFloat = isFloatType(retVal.getTypeInfo());
      if (isFloat) {
        if (retStr != "xmm0") {
          code.push_back(X86Instruction("movsd", "xmm0", retStr));
        }
      } else {
        if (retStr != "rax") {
          code.push_back(X86Instruction("mov", "rax", retStr));
        }
      }
    }

    // Jump to function epilogue.
    code.push_back(X86Instruction("jmp", makeLabel("epilogue")));
  }

  void
  InstructionSelector::selectConversion(const ir::Instruction &instr,
                                        const RegisterAllocator &allocator,
                                        std::vector<X86Instruction> &code) {
    const auto &dest = instr.getDest();
    const auto &src = instr.getOperands()[0];

    std::string destStr = formatOperand(dest, allocator);
    std::string srcStr = formatOperand(src, allocator);

    switch (instr.getOpcode()) {
      using enum ir::Opcode;

      case ITOF:
        code.push_back(X86Instruction("cvtsi2sd", destStr, srcStr));
        break;

      case FTOI:
        code.push_back(X86Instruction("cvttsd2si", destStr, srcStr));
        break;

      default:
        break;
    }
  }

  std::string
  InstructionSelector::formatOperand(const ir::Operand &operand,
                                     const RegisterAllocator &allocator) {
    switch (operand.getType()) {
      using enum ir::OperandType;

      case VREG: {
        uint32_t vreg = operand.getVRegID();

        if (allocator.isSpilled(vreg)) {
          int32_t offset = allocator.getSpillSlot(vreg);
          return "qword [rbp " + std::to_string(offset) + "]";
        }

        PhysReg physReg = allocator.getPhysReg(vreg);
        return getRegName64(physReg);
      }

      case IMMEDIATE:
        return std::to_string(operand.getImmediate());

      case FIMMEDIATE: {
        std::string label = createFloatConstant(operand.getFImmediate());
        return "qword [rel " + label + "]";
      }

      case GLOBAL:
        return "[rel " + operand.getGlobalName() + "]";

      case STACK_SLOT: {
        int32_t offset = operand.getStackOffset();
        return "qword [rbp " + std::to_string(offset) + "]";
      }
    }

    return "";
  }

  std::string InstructionSelector::getRegName64(PhysReg reg) {
    switch (reg) {
      case PhysReg::RAX:
        return "rax";
      case PhysReg::RBX:
        return "rbx";
      case PhysReg::RCX:
        return "rcx";
      case PhysReg::RDX:
        return "rdx";
      case PhysReg::RSI:
        return "rsi";
      case PhysReg::RDI:
        return "rdi";
      case PhysReg::RBP:
        return "rbp";
      case PhysReg::RSP:
        return "rsp";
      case PhysReg::R8:
        return "r8";
      case PhysReg::R9:
        return "r9";
      case PhysReg::R10:
        return "r10";
      case PhysReg::R11:
        return "r11";
      case PhysReg::R12:
        return "r12";
      case PhysReg::R13:
        return "r13";
      case PhysReg::R14:
        return "r14";
      case PhysReg::R15:
        return "r15";
      case PhysReg::XMM0:
        return "xmm0";
      case PhysReg::XMM1:
        return "xmm1";
      case PhysReg::XMM2:
        return "xmm2";
      case PhysReg::XMM3:
        return "xmm3";
      case PhysReg::XMM4:
        return "xmm4";
      case PhysReg::XMM5:
        return "xmm5";
      case PhysReg::XMM6:
        return "xmm6";
      case PhysReg::XMM7:
        return "xmm7";
      case PhysReg::XMM8:
        return "xmm8";
      case PhysReg::XMM9:
        return "xmm9";
      case PhysReg::XMM10:
        return "xmm10";
      case PhysReg::XMM11:
        return "xmm11";
      case PhysReg::XMM12:
        return "xmm12";
      case PhysReg::XMM13:
        return "xmm13";
      case PhysReg::XMM14:
        return "xmm14";
      case PhysReg::XMM15:
        return "xmm15";
      case PhysReg::NONE:
        return "NONE";
    }

    return "UNKNOWN";
  }

  std::string InstructionSelector::createFloatConstant(double value) {
    // Check if we already have this constant.
    for (const auto &fc : floatConstants) {
      if (fc.value == value || (std::isnan(value) && std::isnan(fc.value))) {
        return fc.label;
      }
    }

    // Create new label.
    std::string label = ".LC" + std::to_string(nextFloatLabel++);
    floatConstants.emplace_back(label, value);
    return label;
  }

  std::string
  InstructionSelector::createStringConstant(const std::string &value) {
    // Check if we already have this constant.
    for (const auto &sc : stringConstants) {
      if (sc.value == value) {
        return sc.label;
      }
    }

    // Create new label.
    std::string label = ".str" + std::to_string(nextStringLabel++);
    stringConstants.emplace_back(label, value);
    return label;
  }

  void InstructionSelector::generatePrologue(const ir::Function &func,
                                             const RegisterAllocator &allocator,
                                             FunctionCode &code) {
    // Calculate stack frame size.
    size_t spillSpace = allocator.getSpillStackSize();
    size_t calleeSavedSpace = allocator.getUsedCalleeSaved().size() * 8;

    // Need at least 16 bytes for alignment, plus any spills.
    // Reserve space for local variables that might be stored to stack.
    size_t localSpace = 32;

    // Total stack space (aligned to 16 bytes).
    size_t totalSpace = spillSpace + localSpace + calleeSavedSpace;
    if (totalSpace % 16 != 0) {
      totalSpace += 16 - (totalSpace % 16);
    }

    // Ensure minimum stack allocation.
    if (totalSpace < 16) {
      totalSpace = 16;
    }

    code.stackSize = totalSpace;

    // Push base pointer.
    code.prologue.push_back(X86Instruction("push", "rbp"));
    code.prologue.push_back(X86Instruction("mov", "rbp", "rsp"));

    // Allocate stack space.
    code.prologue.push_back(
        X86Instruction("sub", "rsp", std::to_string(totalSpace)));

    // Save callee-saved registers.
    int32_t offset = -8;
    for (PhysReg reg : allocator.getUsedCalleeSaved()) {
      std::string regName = getRegName64(reg);
      std::string stackLoc = "[rbp " + std::to_string(offset) + "]";
      code.prologue.push_back(
          X86Instruction("mov", stackLoc, regName, "Save " + regName));

      offset -= 8;
    }

    // Move parameters from argument registers to their allocated registers.
    size_t intArgIndex = 0;
    size_t floatArgIndex = 0;

    for (size_t i = 0; i < func.getParams().size(); ++i) {
      const auto &param = func.getParams()[i];
      bool isFloat = isFloatType(param.type);

      // The vreg ID for this parameter matches the parameter index.
      uint32_t paramVreg = static_cast<uint32_t>(i);

      if (allocator.isSpilled(paramVreg)) {
        // Parameter was spilled.
        int32_t spillOffset = allocator.getSpillSlot(paramVreg);
        std::string stackLoc =
            "qword [rbp " + std::to_string(spillOffset) + "]";

        if (isFloat) {
          if (floatArgIndex < 8) {
            std::string srcReg = getRegName64(FLOAT_ARG_REGS[floatArgIndex]);
            code.prologue.push_back(X86Instruction(
                "movsd", stackLoc, srcReg, "Store param " + param.name));
          }

          floatArgIndex++;
        }

        else {
          if (intArgIndex < 6) {
            std::string srcReg = getRegName64(INT_ARG_REGS[intArgIndex]);
            code.prologue.push_back(X86Instruction(
                "mov", stackLoc, srcReg, "Store param " + param.name));
          }

          intArgIndex++;
        }
      }

      else {
        PhysReg destPhysReg = allocator.getPhysReg(paramVreg);
        std::string destReg = getRegName64(destPhysReg);

        if (isFloat) {
          if (floatArgIndex < 8) {
            std::string srcReg = getRegName64(FLOAT_ARG_REGS[floatArgIndex]);
            if (srcReg != destReg) {
              code.prologue.push_back(X86Instruction(
                  "movsd", destReg, srcReg, "Load param " + param.name));
            }
          }

          floatArgIndex++;
        }

        else {
          if (intArgIndex < 6) {
            std::string srcReg = getRegName64(INT_ARG_REGS[intArgIndex]);
            if (srcReg != destReg) {
              code.prologue.push_back(X86Instruction(
                  "mov", destReg, srcReg, "Load param " + param.name));
            }
          }

          intArgIndex++;
        }
      }
    }
  }

  void InstructionSelector::generateEpilogue(const ir::Function &func,
                                             const RegisterAllocator &allocator,
                                             FunctionCode &code) {
    code.epilogue.push_back(X86Instruction(makeLabel("epilogue") + ":"));

    // Restore callee-saved registers in reverse order.
    const auto &usedCalleeSaved = allocator.getUsedCalleeSaved();
    int32_t offset =
        -8 - static_cast<int32_t>((usedCalleeSaved.size() - 1) * 8);

    for (auto iter = usedCalleeSaved.rbegin(); iter != usedCalleeSaved.rend();
         ++iter) {
      std::string regName = getRegName64(*iter);
      std::string stackLoc = "[rbp " + std::to_string(offset) + "]";
      code.epilogue.push_back(
          X86Instruction("mov", regName, stackLoc, "Restore " + regName));

      offset += 8;
    }

    // Restore stack and base pointer.
    code.epilogue.push_back(X86Instruction("mov", "rsp", "rbp"));
    code.epilogue.push_back(X86Instruction("pop", "rbp"));
    code.epilogue.push_back(X86Instruction("ret"));
  }
} // namespace verte::codegen
