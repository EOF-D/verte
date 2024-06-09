/**
 * @brief The codegen implementation.
 * @file codegen.cpp
 */

#include "verte/backend/codegen/codegen.hpp"
#include "verte/errors.hpp"

namespace verte::codegen {
  llvm::Module &Codegen::getModule() const { return *module; }

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
      using enum TypeInfo::DataType;

      case INTEGER: {
        int intValue = std::stoi(value);
        return llvm::ConstantInt::get(context, llvm::APInt(32, intValue, 10));
      }

      case FLOAT:
        return llvm::ConstantFP::get(context, llvm::APFloat(std::stof(value)));

      case DOUBLE:
        return llvm::ConstantFP::get(context, llvm::APFloat(std::stod(value)));

      case BOOL: {
        bool boolValue = value == "true";
        return llvm::ConstantInt::get(context, llvm::APInt(1, boolValue, 10));
      }

      //  TODO: Implement.
      case STRING:
        return createString(value);

      case VOID:
      case UNKNOWN:
        return {};
    }

    llvm_unreachable("Invalid data type.");
  }

  auto Codegen::visit(const VarDeclNode &node) -> RetT {
    auto type = getType(node.getType());
    const std::string &name = node.getName();

    // Handle local definition.
    if (currentFunc != nullptr) {
      auto value = std::get<llvm::Value *>(node.getValue()->accept(*this));
      if (!value)
        error("Invalid value for variable: " + name);

      if (node.isConstant()) {
        currentFunc->constants[name] = llvm::cast<llvm::Constant>(value);
        return {};
      }

      // If the variable is not constant, allocate memory for it.
      auto alloca = builder->CreateAlloca(type, nullptr, name);
      builder->CreateStore(value, alloca);
      currentFunc->locals[name] = alloca;
    }

    // Handle global definition.
    else {
      llvm::Constant *valuePtr = llvm::Constant::getNullValue(type);

      auto value = std::get<llvm::Value *>(node.getValue()->accept(*this));
      if (!value)
        error("Invalid value for variable declaration: " + name);

      if (!node.isConstant())
        error("Global variable must be constant: " + name);

      valuePtr = llvm::cast<llvm::Constant>(value);
      constants[name] = valuePtr; // Register the constant to the codegen.

      // Create the global variable.
      auto globalVar = new llvm::GlobalVariable(
          *module, type, true, llvm::GlobalValue::ExternalLinkage, valuePtr,
          name);

      globals[name] = globalVar;
    }

    return {};
  }

  auto Codegen::visit(const AssignNode &node) -> RetT {
    const std::string &name = node.getName();

    // Check if the variable is a constant.
    if (constants.contains(name))
      error("Cannot assign to a constant: " + name);

    else if (globals.contains(name))
      error("Cannot assign to a global variable: " + name);

    auto value = std::get<llvm::Value *>(node.getValue()->accept(*this));
    if (!value)
      error("Invalid value for assignment: " + name);

    // Checking in function scope.
    if (currentFunc != nullptr) {
      if (currentFunc->constants.find(name) != currentFunc->constants.end())
        error("Cannot assign to constant variable: " + name);

      else if (currentFunc->locals.find(name) == currentFunc->locals.end())
        error("Unknown variable referenced: " + name);

      if (currentFunc->locals.contains(name)) {
        auto alloca = currentFunc->locals[name];
        builder->CreateStore(value, alloca);

        return {};
      }
    }

    // Variable not found in locals, globals.
    error("Unknown variable referenced: " + name);
  }

  auto Codegen::visit(const VariableNode &node) -> RetT {
    const std::string name = node.getName();

    if (globals.contains(name))
      return loadGlobal(name);

    else if (constants.contains(name))
      return constants[name];

    if (currentFunc != nullptr) {
      if (currentFunc->locals.contains(name)) {
        auto alloca = currentFunc->locals[name];
        return builder->CreateLoad(alloca->getAllocatedType(), alloca, name);
      }

      if (currentFunc->constants.contains(name))
        return currentFunc->constants[name];
    }

    error("Unknown variable referenced: " + name);
  }

  auto Codegen::visit(const BinaryNode &node) -> RetT {
    auto lhs = std::get<llvm::Value *>(node.getLHS()->accept(*this));
    auto rhs = std::get<llvm::Value *>(node.getRHS()->accept(*this));
    const std::string &op = node.getOp();

    if (!lhs || !rhs)
      error("Invalid binary operation.");

    llvm::Type *lhsType = lhs->getType();
    llvm::Type *rhsType = rhs->getType();

    // NOTE: Simple type checking. Add a full type checking visitor later.
    if (lhsType != rhsType)
      error("Binary operands must have the same type.");

    // clang-format off
    if (op == "+") return builder->CreateAdd(lhs, rhs, "addtmp");
    else if (op == "-") return builder->CreateSub(lhs, rhs, "subtmp");
    else if (op == "*") return builder->CreateMul(lhs, rhs, "multmp");
    else if (op == "/") return builder->CreateFDiv(lhs, rhs, "divtmp");
    else if (op == "<") return builder->CreateICmpULT(lhs, rhs, "cmptmp");
    else if (op == ">") return builder->CreateICmpUGT(lhs, rhs, "cmptmp");
    else if (op == "==") return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
    else if (op == "!=") return builder->CreateICmpNE(lhs, rhs, "cmptmp");
    else if (op == "<=") return builder->CreateICmpULE(lhs, rhs, "cmptmp");
    else if (op == ">=") return builder->CreateICmpUGE(lhs, rhs, "cmptmp");
    // clang-format on

    error("Invalid binary operator: " + op);
  }

  auto Codegen::visit(const UnaryNode &node) -> RetT {
    const std::string &op = node.getOp();
    auto operand = std::get<llvm::Value *>(node.getOperand()->accept(*this));

    if (!operand)
      error("Invalid operand for unary operation");

    if (op == "-")
      return builder->CreateNeg(operand, "negtmp");

    else if (op == "!")
      return builder->CreateNot(operand, "nottmp");

    error("Invalid unary operator: " + op);
  }

  auto Codegen::visit(const ProtoNode &node) -> RetT {
    const std::string name = node.getName();

    // Get parameter types.
    std::vector<llvm::Type *> paramTypes;
    for (const auto &param : node.getParams())
      paramTypes.push_back(getType(param.type));

    // Create the function type.
    llvm::Type *returnType = getType(node.getRetType());
    llvm::FunctionType *funcType =
        llvm::FunctionType::get(returnType, paramTypes, false);

    // Create the function.
    llvm::Function *func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, name, module.get());

    // Set the names for the function arguments.
    size_t i = 0;
    for (auto &arg : func->args())
      arg.setName(node.getParams()[i++].name);

    return func;
  }

  auto Codegen::visit(const BlockNode &node) -> RetT {
    // Visit the children nodes.
    for (const auto &child : node.getBody())
      child->accept(*this);

    return {};
  }

  auto Codegen::visit(const FuncDeclNode &node) -> RetT {
    llvm::Function *func =
        std::get<llvm::Function *>(node.getProto()->accept(*this));

    // Saving the previous function.
    std::unique_ptr<Function> prev = std::move(currentFunc);

    // Update the current function.
    const std::string &name = node.getProto()->getName();
    std::vector<llvm::Type *> paramTypes = func->getFunctionType()->params();
    llvm::Type *retType = func->getReturnType();

    currentFunc =
        std::make_unique<Function>(Function(name, paramTypes, retType));

    currentFunc->llvmFunc = func;

    // Create the entry block.
    llvm::BasicBlock *block = llvm::BasicBlock::Create(context, "entry", func);
    builder->SetInsertPoint(block);

    // Make the arguments available in the function.
    for (auto &arg : func->args()) {
      llvm::AllocaInst *allocaInst =
          builder->CreateAlloca(arg.getType(), nullptr, arg.getName());

      builder->CreateStore(&arg, allocaInst);
      currentFunc->locals[arg.getName().str()] = allocaInst;
    }

    // Visit the function body.
    node.getBody()->accept(*this);

    // Reset the current function.
    currentFunc = std::move(prev);
    return func;
  }

  auto Codegen::visit(const CallNode &node) -> RetT {
    // Get the callee function.
    std::string name = node.getCallee()->getName();
    llvm::Function *callee = module->getFunction(name);

    if (!callee)
      error("Unknown function referenced: " + name);

    // Get the arguments.
    std::vector<llvm::Value *> args;
    for (const auto &arg : node.getArgs())
      args.push_back(std::get<llvm::Value *>(arg->accept(*this)));

    // Create the call instruction.
    return builder->CreateCall(callee, args, "calltmp");
  }

  auto Codegen::visit(const ReturnNode &node) -> RetT {
    llvm::Value *ret = std::get<llvm::Value *>(node.getValue()->accept(*this));

    // Create the return instruction.
    builder->CreateRet(ret);
    return {};
  }

  llvm::Type *Codegen::getType(const TypeInfo &type) const {
    switch (type.dataType) {
      case TypeInfo::DataType::INTEGER:
        return builder->getInt32Ty();

      case TypeInfo::DataType::FLOAT:
        return builder->getFloatTy();

      case TypeInfo::DataType::DOUBLE:
        return builder->getDoubleTy();

      case TypeInfo::DataType::BOOL:
        return builder->getInt1Ty();

      case TypeInfo::DataType::STRING:
        return builder->getInt8PtrTy();

      case TypeInfo::DataType::VOID:
        return builder->getVoidTy();

      default:
        return nullptr;
    }
  }

  llvm::Value *Codegen::loadGlobal(const std::string &name) {
    if (globals.contains(name)) {
      auto globalVar = globals[name];
      return builder->CreateLoad(globalVar->getValueType(), globalVar, name);
    }

    error("Unknown global variable: " + name);
  }

  llvm::Value *Codegen::createString(const std::string &value) {
    auto *strConst = llvm::ConstantDataArray::getString(context, value, true);

    llvm::GlobalVariable *str = new llvm::GlobalVariable(
        *module, strConst->getType(), true, llvm::GlobalValue::PrivateLinkage,
        strConst, "str");

    return builder->CreatePointerCast(str, llvm::Type::getInt8PtrTy(context));
  }

  template <typename... Args>
  [[noreturn]] void Codegen::error(const std::string &message, Args &&...args) {
    logger.error(message, std::forward<Args>(args)...); // Log then throw.
    throw errors::CodegenError(message);
  }
} // namespace verte::codegen
