/**
 * @brief AST visitor definition.
 * @file base.hpp
 */

#ifndef VERTE_FRONTEND_VISITORS_BASE_HPP
#define VERTE_FRONTEND_VISITORS_BASE_HPP

#include "verte/frontend/parser/ast.hpp"

/**
 * @namespace verte::visitors
 * @brief The visitors namespace. Contains AST visitors.
 */
namespace verte::visitors {
  using namespace verte::types;
  using namespace verte::nodes;

  /**
   * @class ASTVisitor
   * @brief Base AST visitor class.
   */
  class ASTVisitor {
  public:
    /**
     * @brief Default destructor.
     */
    virtual ~ASTVisitor() = default;

    /**
     * @brief Visit the program node.
     * @param node The program node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const ProgramNode &node) -> RetT = 0;

    /**
     * @brief Visit a literal node.
     * @param node The literal node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const LiteralNode &node) -> RetT = 0;

    /**
     * @brief Visit a variable declaration node.
     * @param node The variable declaration node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const VarDeclNode &node) -> RetT = 0;

    /**
     * @brief Visit an assignment node.
     * @param node The assignment node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const AssignNode &node) -> RetT = 0;

    /**
     * @brief Visit a variable node.
     * @param node The variable node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const VariableNode &node) -> RetT = 0;

    /**
     * @brief Visit a binary operation node.
     * @param node The binary operation node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const BinaryNode &node) -> RetT = 0;

    /**
     * @brief Visit a unary operation node.
     * @param node The unary operation node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const UnaryNode &node) -> RetT = 0;

    /**
     * @brief Visit a proto node.
     * @param node The proto node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const ProtoNode &node) -> RetT = 0;

    /**
     * @brief Visit a block node.
     * @param node The block node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const BlockNode &node) -> RetT = 0;

    /**
     * @brief Visit a function declaration node.
     * @param node The function declaration node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const FuncDeclNode &node) -> RetT = 0;

    /**
     * @brief Visit a function call node.
     * @param node The function call node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const CallNode &node) -> RetT = 0;

    /**
     * @brief Visit a return node.
     * @param node The return node to visit.
     * @return The return value of the visit.
     */
    virtual auto visit(const ReturnNode &node) -> RetT = 0;
  };
} // namespace verte::visitors

#endif // VERTE_FRONTEND_VISITORS_BASE_HPP
