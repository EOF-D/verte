/**
 * @brief Code generation interface.
 * @file codegen.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_CODEGEN_HPP
#define VERTE_BACKEND_CODEGEN_CODEGEN_HPP

#include "verte/backend/ir/basic_block.hpp"
#include "verte/backend/ir/function.hpp"
#include "verte/backend/ir/module.hpp"
#include "verte/backend/ir/operand.hpp"
#include "verte/frontend/visitors/base.hpp"
#include "verte/utils/logger.hpp"

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
     * @param moduleName Name of the IR module.
     */
    explicit Codegen(const std::string &moduleName)
        : module(moduleName), nextVReg(0), currentFunc(nullptr),
          currentBlock(nullptr), logger("codegen") {
      initTable();
    }

    /**
     * @brief Get the module used.
     * @return The module.
     */
    ir::Module &getModule() { return module; }

    /**
     * @brief Get the module used (const).
     * @return The module.
     */
    const ir::Module &getModule() const { return module; }

    /**
     * @brief Visit a ProgramNode.
     * @param node The ProgramNode to visit.
     */
    auto visit(const ProgramNode &node) -> RetT override;

    /**
     * @brief Visit a LiteralNode.
     * @param node The LiteralNode to visit.
     * @return The generated IR operand.
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
     * @return The generated IR operand.
     */
    auto visit(const AssignNode &node) -> RetT override;

    /**
     * @brief Visit a VariableNode.
     * @param node The VariableNode to visit.
     * @return The generated IR operand.
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
     * @return The generated IR operand.
     */
    auto visit(const BinaryNode &node) -> RetT override;

    /**
     * @brief Visit a UnaryNode.
     * @param node The UnaryNode to visit.
     * @return The generated IR operand.
     */
    auto visit(const UnaryNode &node) -> RetT override;

    /**
     * @brief Visit a ProtoNode.
     * @param node The ProtoNode to visit.
     * @return The generated IR function pointer.
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
     * @return The generated IR function pointer.
     */
    auto visit(const FuncDeclNode &node) -> RetT override;

    /**
     * @brief Visit a CallNode.
     * @param node The CallNode to visit.
     * @return The generated IR operand.
     */
    auto visit(const CallNode &node) -> RetT override;

    /**
     * @brief Visit a ReturnNode.
     * @param node The ReturnNode to visit.
     */
    auto visit(const ReturnNode &node) -> RetT override;

  private:
    /**
     * @brief Allocate a new virtual register.
     * @param type The type of the register.
     * @return A new virtual register operand.
     */
    ir::Operand allocateVReg(const types::TypeInfo &type) {
      return ir::Operand::vreg(nextVReg++, type);
    }

    /**
     * @brief Create a new basic block in the current function.
     * @param label The label of the basic block.
     * @return Pointer to the created basic block.
     */
    ir::BasicBlock *createBlock(const std::string &label);

    /**
     * @brief Create a string constant.
     * @param value The string value.
     * @return The created string operand.
     */
    ir::Operand createString(const std::string &value);

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
      // Add true & false to the global constants table.
      globalConstants["true"] = ir::Operand::imm(1);
      globalConstants["false"] = ir::Operand::imm(0);
    }

    ir::Module module; /**< IR module. */

    uint32_t nextVReg;            /**< Next virtual register id. */
    ir::Function *currentFunc;    /**< Current function being processed. */
    ir::BasicBlock *currentBlock; /**< Current basic block being processed. */

    std::unordered_map<std::string, ir::Operand>
        constants; /**< Constants in current function. */

    std::unordered_map<std::string, ir::Operand>
        locals; /**< Local variables in current function. */

    std::unordered_map<std::string, ir::Operand>
        globalConstants; /**< Global constants. */

    std::unordered_map<std::string, ir::Operand>
        globals; /**< Global variables. */

    std::unordered_map<std::string, ir::Operand>
        strings; /**< String literals. */

    utils::Logger logger; /**< The logger. */
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_CODEGEN_HPP
