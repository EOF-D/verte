/**
 * @brief AST pretty printer definition.
 * @file ast_printer.hpp
 */

#ifndef VERTE_FRONTEND_VISITORS_AST_PRINTER_HPP
#define VERTE_FRONTEND_VISITORS_AST_PRINTER_HPP

#include "verte/frontend/visitors/base.hpp"

#include <iostream>

/**
 * @namespace verte::visitors
 * @brief The visitors namespace. Contains AST visitors.
 */
namespace verte::visitors {
  /**
   * @brief Concept to check if a type supports the `<<` operator.
   * @tparam T The type to check.
   */
  template <typename T>
  concept OutputStream =
      requires(T &stream, typename T::char_type c) { stream << c; };

  /**
   * @class ASTPrinter
   * @brief The ASTPrinter class is responsible for printing the AST in
   * readable form.
   */
  class ASTPrinter : public ASTVisitor {
  public:
    /**
     * @brief Construct a new ASTPrinter.
     */
    ASTPrinter() : stream(std::cout) {}

    /**
     * @brief Construct a new ASTPrinter.
     * @tparam Stream The output stream type.
     * @param stream The output stream.
     */
    template <OutputStream Stream>
    ASTPrinter(Stream &stream) : stream(stream) {}

    /**
     * @brief Visit a ProgramAST node.
     * @param node The ProgramAST node to visit.
     */
    auto visit(const ProgramNode &node) -> RetT override;

    /**
     * @brief Visit a LiteralNode node.
     * @param node The LiteralNode node to visit.
     */
    auto visit(const LiteralNode &node) -> RetT override;

    /**
     * @brief Visit a VarDeclNode node.
     * @param node The VarDeclNode node to visit.
     */
    auto visit(const VarDeclNode &node) -> RetT override;

    /**
     * @brief Visit an AssignNode node.
     * @param node The AssignNode node to visit.
     */
    auto visit(const AssignNode &node) -> RetT override;

    /**
     * @brief Visit a VariableNode node.
     * @param node The VariableNode node to visit.
     */
    auto visit(const VariableNode &node) -> RetT override;

    /**
     * @brioef Visit a IfNode node.
     * @param node The IfNode node to visit.
     */
    auto visit(const IfNode &node) -> RetT override;

    /**
     * @brief Visit a IfElseNode node.
     * @param node The IfElseNode node to visit.
     */
    auto visit(const IfElseNode &node) -> RetT override;

    /**
     * @brief Visit a BinaryNode node.
     * @param node The BinaryNode node to visit.
     */
    auto visit(const BinaryNode &node) -> RetT override;

    /**
     * @brief Visit a UnaryNode node.
     * @param node The UnaryNode node to visit.
     */
    auto visit(const UnaryNode &node) -> RetT override;

    /**
     * @brief Visit a ProtoNode node.
     * @param node The ProtoNode node to visit.
     */
    auto visit(const ProtoNode &node) -> RetT override;

    /**
     * @brief Visit a BlockNode node.
     * @param node The BlockNode node to visit.
     */
    auto visit(const BlockNode &node) -> RetT override;

    /**
     * @brief Visit a FuncDeclNode node.
     * @param node The FuncDeclNode node to visit.
     */
    auto visit(const FuncDeclNode &node) -> RetT override;

    /**
     * @brief Visit a CallNode node.
     * @param node The CallNode node to visit.
     */
    auto visit(const CallNode &node) -> RetT override;

    /**
     * @brief Visit a ReturnNode node.
     * @param node The ReturnNode node to visit.
     */
    auto visit(const ReturnNode &node) -> RetT override;

  private:
    /**
     * @brief Print the current indentation level.
     * @return The output stream with the indentation printed.
     */
    std::ostream &printIndent() {
      for (int i = 0; i < indentLevel * 2; ++i) {
        stream << ' ';
      }

      return stream;
    }

    /**
     * @brief The IndentGuard class is a helper class used to manage the
     * indentation level in the ASTPrinter.
     */
    class IndentGuard {
    public:
      /**
       * @brief Construct a new IndentGuard.
       * @param printer The ASTPrinter.
       */
      explicit IndentGuard(ASTPrinter &printer) : printer(printer) {
        printer.indentLevel++;
      }

      /**
       * @brief Destroy the IndentGuard.
       */
      ~IndentGuard() { printer.indentLevel--; }

    private:
      ASTPrinter &printer; /**< The ASTPrinter. */
    };

    // Allow the IndentGuard class to access the private members.
    friend class IndentGuard;

    int indentLevel = 0;  /**< The current indentation level. */
    std::ostream &stream; /**< The output stream. */
  };
} // namespace verte::visitors

#endif // VERTE_FRONTEND_VISITORS_AST_PRINTER_HPP
