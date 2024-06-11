/**
 * @brief Code generation interface.
 * @file codegen.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_CODEGEN_HPP
#define VERTE_BACKEND_CODEGEN_CODEGEN_HPP

#include "verte/frontend/visitors/base.hpp"
#include "verte/utils/logger.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  /**
   * @typedef ModulePtr
   * @brief Unique pointer to an LLVM module.
   */
  using ModulePtr = std::unique_ptr<llvm::Module>;

  /**
   * @typedef BuilderPtr
   * @brief Unique pointer to an LLVM IR builder.
   */
  using BuilderPtr = std::unique_ptr<llvm::IRBuilder<>>;
} // namespace verte::codegen

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  using namespace verte::nodes;

  /**
   * @class Codegen
   * @brief Code generation visitor.
   */
  class Codegen : public visitors::ASTVisitor {
  public:
    /**
     * @brief Construct a new Codegen.
     * @param context LLVM context.
     * @param module LLVM module.
     */
    Codegen(llvm::LLVMContext &context, ModulePtr module)
        : context(context), currentFunc(), logger("codegen") {
      this->builder = std::make_unique<llvm::IRBuilder<>>(context);
      this->module = std::move(module);
      initTable();
    }

    /**
     * @brief Get the module used.
     * @return The module.
     */
    llvm::Module &getModule() const;

    /**
     * @brief Visit a ProgramNode.
     * @param node The ProgramNode to visit.
     */
    auto visit(const ProgramNode &node) -> RetT override;

    /**
     * @brief Visit a LiteralNode.
     * @param node The LiteralNode to visit.
     * @return The generated LLVM value.
     */
    auto visit(const LiteralNode &node) -> RetT override;

    /**
     * @brief Visit a VarDeclNode.
     * @param node The VarDeclNode to visit.
     */
    auto visit(const VarDeclNode &node) -> RetT override;

    /**
     * @brief Visit a AssignNode.
     * @param node The AssignNode to visit.
     * @return The generated LLVM value.
     */
    auto visit(const AssignNode &node) -> RetT override;

    /**
     * @brief Visit a VariableNode.
     * @param node The VariableNode to visit.
     * @return The generated LLVM value.
     */
    auto visit(const VariableNode &node) -> RetT override;

    /**
     * @brief Visit a IfNode.
     * @param node The IfNode to visit.
     */
    auto visit(const IfNode &node) -> RetT override;

    /**
     * @brief Visit a IfElseNode.
     * @param node The IfElseNode to visit.
     */
    auto visit(const IfElseNode &node) -> RetT override;

    /**
     * @brief Visit a BinaryNode.
     * @param node The BinaryNode to visit.
     * @return The generated LLVM value.
     */
    auto visit(const BinaryNode &node) -> RetT override;

    /**
     * @brief Visit a UnaryNode.
     * @param node The UnaryNode to visit.
     * @return The generated LLVM value.
     */
    auto visit(const UnaryNode &node) -> RetT override;

    /**
     * @brief Visit a ProtoNode.
     * @param node The ProtoNode to visit.
     * @return The generated LLVM function.
     */
    auto visit(const ProtoNode &node) -> RetT override;

    /**
     * @brief Visit a BlockNode.
     * @param node The BlockNode to visit.
     */
    auto visit(const BlockNode &node) -> RetT override;

    /**
     * @brief Visit a FuncDeclNode.
     * @param node The FuncDeclNode to visit.
     * @return The generated LLVM function.
     */
    auto visit(const FuncDeclNode &node) -> RetT override;

    /**
     * @brief Visit a CallNode.
     * @param node The CallNode to visit.
     * @return The generated LLVM value.
     */
    auto visit(const CallNode &node) -> RetT override;

    /**
     * @brief Visit a ReturnNode.
     * @param node The ReturnNode to visit.
     */
    auto visit(const ReturnNode &node) -> RetT override;

  private:
    /**
     * @brief Get the LLVM type for a given TypeInfo.
     * @param type The TypeInfo to convert.
     * @return The corresponding LLVM type.
     */
    llvm::Type *getType(const TypeInfo &type) const;

    /**
     * @brief Load a global variable.
     * @param name The name of the global variable.
     * @return The loaded LLVM value.
     */
    llvm::Value *loadGlobal(const std::string &name);

    /**
     * @brief Create a string.
     * @param value The string value.
     * @return The created LLVM value.
     */
    llvm::Value *createString(const std::string &value);

    /**
     * @brief Emit an error message and exit.
     * @tparam Args Argument types.
     * @param message The error message.
     * @param args Additional arguments.
     */
    template <typename... Args>
    [[noreturn]] void error(const std::string &message, Args &&...args);

    /**
     * @brief Initialize the symbol table with some constants, etc.
     */
    void initTable() {
      // Add true & false to the global table.
      constants["true"] = llvm::ConstantInt::getTrue(context);
      constants["false"] = llvm::ConstantInt::getFalse(context);

      // TODO: Make preloading of functions have a better interface.
      std::vector<llvm::Type *> printArgs{builder->getInt8PtrTy()};
      auto printType =
          llvm::FunctionType::get(builder->getInt32Ty(), printArgs, true);

      auto func = llvm::Function::Create(
          printType, llvm::Function::ExternalLinkage, "printf", module.get());

      func->setCallingConv(llvm::CallingConv::C);
    }

    llvm::LLVMContext &context; /**< LLVM context. */
    ModulePtr module;           /**< LLVM module. */
    BuilderPtr builder;         /**< LLVM IR builder. */

    std::unique_ptr<types::Function>
        currentFunc; /**< Current function being processed. */

    std::unordered_map<std::string, llvm::Constant *>
        constants; /**< Constants, i.e true/false. */

    std::unordered_map<std::string, llvm::GlobalVariable *>
        globals; /**< Global variables. */

    utils::Logger logger; /**< The logger. */
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_CODEGEN_HPP
