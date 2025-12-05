/**
 * @brief The codegen implementation.
 * @file codegen.cpp
 */

#include "verte/backend/codegen/codegen.hpp"
#include "verte/errors.hpp"

namespace verte::codegen {
  auto Codegen::visit(const ProgramNode &node) -> RetT {
    for (const auto &child : node.getBody()) {
      child->accept(*this);
    }

    return {};
  }

  auto Codegen::visit(const LiteralNode &node) -> RetT {
    const auto &value = node.getValue();
    const auto &typeInfo = node.getType();

    switch (typeInfo.dataType) {
      using enum types::TypeInfo::DataType;

      case INTEGER: {
        int64_t intValue = std::stoll(value);
        return ir::Operand::imm(intValue);
      }

      case FLOAT:
      case DOUBLE: {
        double doubleValue = std::stod(value);
        return ir::Operand::fimm(doubleValue);
      }

      case BOOL: {
        return ir::Operand::imm(value == "true" ? 1 : 0);
      }

      case STRING: {
        return createString(value);
      }

      case VOID:
      case UNKNOWN:
        error("Invalid literal type");
    }

    // Should not reach here.
    error("Invalid literal type");
  }

  auto Codegen::visit(const VarDeclNode &node) -> RetT {
    const std::string &name = node.getName();
    const auto &type = node.getType();

    // Handle local definitions inside of function.
    if (currentFunc != nullptr) {
      auto value = std::get<ir::Operand>(node.getValue()->accept(*this));

      // Handle local constants.
      if (node.isConstant()) {
        constants[name] = value;
        return {};
      }

      // Handle local variables by allocating a stack slot.
      // Use 8-byte alignment for simplicity, and negative offsets because stack
      // grows downward.
      int32_t offset = -static_cast<int32_t>((locals.size() + 1) * 8);
      auto stackSlot = ir::Operand::stackSlot(offset, type);

      // Add store instruction.
      currentBlock->addInstruction(
          ir::Instruction(ir::Opcode::STORE, stackSlot, value));

      locals[name] = stackSlot;
      return {};
    }

    // Handle global definitions.
    auto value = std::get<ir::Operand>(node.getValue()->accept(*this));

    if (!node.isConstant()) {
      error("Global variable must be constant: " + name);
    }

    // Store direct value in global constants.
    // And store global operand for reference.
    globalConstants[name] = value;
    globals[name] = ir::Operand::global(name);

    return {};
  }

  auto Codegen::visit(const AssignNode &node) -> RetT {
    const std::string &name = node.getName();

    // Check global constants/variables.
    if (globalConstants.contains(name)) {
      error("Cannot assign to global constant: " + name);
    }

    if (globals.contains(name)) {
      error("Cannot assign to global variable: " + name);
    }

    // Must be in function scope.
    if (currentFunc == nullptr) {
      error("Assignment must be inside a function: " + name);
    }

    if (constants.contains(name)) {
      error("Cannot assign to constant: " + name);
    }

    // Check if it exists in locals.
    if (!locals.contains(name)) {
      error("Unknown variable referenced: " + name);
    }

    // Check if it's a parameter.
    if (locals[name].isVReg()) {
      error("Cannot assign to parameter: " + name);
    }

    // Must be a stack slot.
    auto value = std::get<ir::Operand>(node.getValue()->accept(*this));
    auto stackSlot = locals[name];
    currentBlock->addInstruction(
        ir::Instruction(ir::Opcode::STORE, stackSlot, value));

    return {};
  }

  auto Codegen::visit(const VariableNode &node) -> RetT {
    const std::string &name = node.getName();

    // Check global scope.
    if (globalConstants.contains(name)) {
      return globalConstants[name];
    }

    if (globals.contains(name)) {
      auto global = globals[name];
      auto result = allocateVReg(global.getTypeInfo());
      currentBlock->addInstruction(
          ir::Instruction(ir::Opcode::LOAD, result, global));

      return result;
    }

    // Check function scope.
    if (currentFunc != nullptr) {
      if (constants.contains(name)) {
        return constants[name];
      }

      if (locals.contains(name)) {
        auto operand = locals[name];

        // If it's a vreg, use directly.
        if (operand.isVReg()) {
          return operand;
        }

        // If it's a stack slot, load it.
        if (operand.isStackSlot()) {
          auto result = allocateVReg(operand.getTypeInfo());
          currentBlock->addInstruction(
              ir::Instruction(ir::Opcode::LOAD, result, operand));

          return result;
        }
      }
    }

    error("Unknown variable referenced: " + name);
  }

  auto Codegen::visit(const IfNode &node) -> RetT {
    if (currentFunc == nullptr)
      error("If statement must be inside a function.");

    auto *condBlock = currentFunc->createBlock("cond");
    auto *thenBlock = currentFunc->createBlock("then");
    auto *mergeBlock = currentFunc->createBlock("merge");

    // Get current block before switching.
    auto *prevBlock = currentBlock;

    // Branch from previous block to condition block.
    prevBlock->setTerminator(
        ir::Instruction(ir::Opcode::BR, condBlock->getLabel()));

    prevBlock->addSuccessor(condBlock);
    condBlock->addPredecessor(prevBlock);

    // Condition block.
    currentBlock = condBlock;
    auto condValue = std::get<ir::Operand>(node.getCond()->accept(*this));
    currentBlock->setTerminator(
        ir::Instruction(ir::Opcode::CONDBR, thenBlock->getLabel(), condValue,
                        mergeBlock->getLabel()));

    currentBlock->addSuccessor(thenBlock);
    currentBlock->addSuccessor(mergeBlock);
    thenBlock->addPredecessor(currentBlock);
    mergeBlock->addPredecessor(currentBlock);

    // Then block.
    currentBlock = thenBlock;
    node.getBlock()->accept(*this);

    // Only add branch if current block doesn't have a terminator.
    if (!currentBlock->hasTerminator()) {
      currentBlock->setTerminator(
          ir::Instruction(ir::Opcode::BR, mergeBlock->getLabel()));

      currentBlock->addSuccessor(mergeBlock);
      mergeBlock->addPredecessor(currentBlock);
    }

    // Merge block.
    currentBlock = mergeBlock;
    return {};
  }

  auto Codegen::visit(const IfElseNode &node) -> RetT {
    if (currentFunc == nullptr)
      error("If-else statement must be inside a function.");

    auto *condBlock = currentFunc->createBlock("cond");
    auto *thenBlock = currentFunc->createBlock("then");
    auto *elseBlock = currentFunc->createBlock("else");
    auto *mergeBlock = currentFunc->createBlock("merge");

    // Get current block before switching.
    auto *prevBlock = currentBlock;

    // Branch from previous block to condition block.
    prevBlock->setTerminator(
        ir::Instruction(ir::Opcode::BR, condBlock->getLabel()));
    prevBlock->addSuccessor(condBlock);
    condBlock->addPredecessor(prevBlock);

    // Condition block.
    currentBlock = condBlock;
    auto condValue =
        std::get<ir::Operand>(node.getIfNode()->getCond()->accept(*this));

    currentBlock->setTerminator(
        ir::Instruction(ir::Opcode::CONDBR, thenBlock->getLabel(), condValue,
                        elseBlock->getLabel()));

    currentBlock->addSuccessor(thenBlock);
    currentBlock->addSuccessor(elseBlock);
    thenBlock->addPredecessor(currentBlock);
    elseBlock->addPredecessor(currentBlock);

    // Then block.
    currentBlock = thenBlock;
    node.getIfNode()->getBlock()->accept(*this);

    // Only add branch if current block doesn't have a terminator.
    if (!currentBlock->hasTerminator()) {
      currentBlock->setTerminator(
          ir::Instruction(ir::Opcode::BR, mergeBlock->getLabel()));

      currentBlock->addSuccessor(mergeBlock);
      mergeBlock->addPredecessor(currentBlock);
    }

    // Else block.
    currentBlock = elseBlock;
    node.getElseBlock()->accept(*this);

    // Only add branch if current block doesn't have a terminator.
    if (!currentBlock->hasTerminator()) {
      currentBlock->setTerminator(
          ir::Instruction(ir::Opcode::BR, mergeBlock->getLabel()));

      currentBlock->addSuccessor(mergeBlock);
      mergeBlock->addPredecessor(currentBlock);
    }

    // Merge block.
    currentBlock = mergeBlock;
    return {};
  }

  auto Codegen::visit(const BinaryNode &node) -> RetT {
    auto lhs = std::get<ir::Operand>(node.getLHS()->accept(*this));
    auto rhs = std::get<ir::Operand>(node.getRHS()->accept(*this));
    const std::string &op = node.getOp();

    // Allocate result register.
    auto result = allocateVReg(lhs.getTypeInfo());

    // Map operator to opcode.
    ir::Opcode opcode;

    if (op == "+")
      opcode = ir::Opcode::ADD;
    else if (op == "-")
      opcode = ir::Opcode::SUB;
    else if (op == "*")
      opcode = ir::Opcode::MUL;
    else if (op == "/")
      opcode = ir::Opcode::DIV;
    else if (op == "%")
      opcode = ir::Opcode::MOD;
    else if (op == "==")
      opcode = ir::Opcode::ICMP_EQ;
    else if (op == "!=")
      opcode = ir::Opcode::ICMP_NE;
    else if (op == "<")
      opcode = ir::Opcode::ICMP_LT;
    else if (op == "<=")
      opcode = ir::Opcode::ICMP_LE;
    else if (op == ">")
      opcode = ir::Opcode::ICMP_GT;
    else if (op == ">=")
      opcode = ir::Opcode::ICMP_GE;
    else
      error("Invalid binary operator: " + op);

    // Add instruction.
    currentBlock->addInstruction(ir::Instruction(opcode, result, lhs, rhs));
    return result;
  }

  auto Codegen::visit(const UnaryNode &node) -> RetT {
    const std::string &op = node.getOp();
    auto operand = std::get<ir::Operand>(node.getOperand()->accept(*this));

    // Allocate result register.
    auto result = allocateVReg(operand.getTypeInfo());

    // Map operator to opcode.
    ir::Opcode opcode;

    if (op == "-")
      opcode = ir::Opcode::NEG;
    else if (op == "!")
      opcode = ir::Opcode::NOT;
    else
      error("Invalid unary operator: " + op);

    // Add instruction.
    currentBlock->addInstruction(ir::Instruction(opcode, result, operand));
    return result;
  }

  auto Codegen::visit(const ProtoNode &node) -> RetT {
    auto params = node.getParams();
    auto *func = module.createFunction(node.getName(), std::move(params),
                                       node.getRetType());

    return func;
  }

  auto Codegen::visit(const BlockNode &node) -> RetT {
    for (const auto &stmt : node.getBody()) {
      stmt->accept(*this);
    }

    return {};
  }

  auto Codegen::visit(const FuncDeclNode &node) -> RetT {
    auto *irFunc = std::get<ir::Function *>(node.getProto()->accept(*this));

    // Save previous function state.
    ir::Function *prevFunc = currentFunc;
    ir::BasicBlock *prevBlock = currentBlock;
    auto prevConstants = std::move(constants);
    auto prevLocals = std::move(locals);
    uint32_t prevVReg = nextVReg;

    // Set current function.
    currentFunc = irFunc;
    nextVReg = 0;
    constants.clear();
    locals.clear();

    // Create entry block and set as current.
    currentBlock = currentFunc->createBlock("entry");

    // Store parameters as virtual registers.
    for (const auto &param : node.getProto()->getParams()) {
      auto paramReg = allocateVReg(param.type);
      locals[param.name] = paramReg;
    }

    // Visit function body.
    node.getBody()->accept(*this);

    // Restore previous function state.
    currentFunc = prevFunc;
    currentBlock = prevBlock;
    constants = std::move(prevConstants);
    locals = std::move(prevLocals);
    nextVReg = prevVReg;

    return irFunc;
  }

  auto Codegen::visit(const CallNode &node) -> RetT {
    const std::string &calleeName = node.getCallee()->getName();

    // Check if it's an external function.
    if (externalFunctions.contains(calleeName)) {
      std::vector<ir::Operand> args;
      for (const auto &arg : node.getArgs()) {
        args.push_back(std::get<ir::Operand>(arg->accept(*this)));
      }

      // Allocate result register (assuming int return for printf).
      auto result =
          allocateVReg(types::TypeInfo(types::TypeInfo::DataType::INTEGER));

      // Add call instruction.
      currentBlock->addInstruction(
          ir::Instruction(result, calleeName, std::move(args)));

      return result;
    }

    // Handle regular function calls.
    if (!module.hasFunction(calleeName)) {
      error("Unknown function: " + calleeName);
    }

    auto *calleeFunc = module.getFunction(calleeName);

    // Generate arguments.
    std::vector<ir::Operand> args;
    for (const auto &arg : node.getArgs()) {
      args.push_back(std::get<ir::Operand>(arg->accept(*this)));
    }

    // Allocate result register.
    auto result = allocateVReg(calleeFunc->getReturnType());

    // Add call instruction.
    currentBlock->addInstruction(
        ir::Instruction(result, calleeName, std::move(args)));

    return result;
  }

  auto Codegen::visit(const ReturnNode &node) -> RetT {
    if (!currentFunc) {
      error("Return statement must be inside a function");
    }

    // Generate return value.
    auto value = std::get<ir::Operand>(node.getValue()->accept(*this));

    // Add return instruction.
    currentBlock->setTerminator(
        ir::Instruction(std::optional<ir::Operand>(value)));

    return {};
  }

  ir::BasicBlock *Codegen::createBlock(const std::string &label) {
    if (!currentFunc) {
      error("Cannot create block outside of function");
    }

    return currentFunc->createBlock(label);
  }

  ir::Operand Codegen::createString(const std::string &value) {
    // Check if we've already created this string literal.
    if (strings.contains(value)) {
      return strings[value];
    }

    // Generate label for string literal.
    std::string label = ".str" + std::to_string(strings.size());

    // Create global operand for string.
    auto global = ir::Operand::global(label);

    // Store the string.
    strings[value] = global;
    globals[label] = global;

    return global;
  }

  template <typename... Args>
  [[noreturn]] void Codegen::error(const std::string &message, Args &&...args) {
    logger.error(message, std::forward<Args>(args)...); // Log then throw.
    throw errors::CodegenError(message);
  }
} // namespace verte::codegen
