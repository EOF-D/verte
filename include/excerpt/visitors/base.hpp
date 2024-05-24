/**
 * @file base.hpp
 * @brief Base visitor class.
 */

#ifndef EXCERPT_BASE_HPP
#define EXCERPT_BASE_HPP

#include "excerpt/parser/ast.hpp"

// clang-format off
namespace excerpt {
  #define DECLARE_VISIT(NodeType) \
    virtual void visit(NodeType &node) { /* default */ } 

  /**
   * @brief Base visitor class.
   */
  // clang-format on
  class ASTVisitor {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~ASTVisitor() = default;

    // Declaring visit methods for each AST node.
    DECLARE_VISIT(ProgramAST)
    DECLARE_VISIT(LiteralNode)
    DECLARE_VISIT(VarDeclNode)
    DECLARE_VISIT(IdentNode)
    DECLARE_VISIT(BinaryNode)
    DECLARE_VISIT(UnaryNode)
    DECLARE_VISIT(ProtoNode)
    DECLARE_VISIT(BlockNode)
    DECLARE_VISIT(FuncDeclNode)
    DECLARE_VISIT(ReturnNode)
    DECLARE_VISIT(CallNode)
  };
  // clang-format off

  #undef DECLARE_VISIT
} // namespace excerpt

#endif // EXCERPT_BASE_HPP
