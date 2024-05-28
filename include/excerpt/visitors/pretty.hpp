/**
 * @file pretty.hpp
 * @brief Pretty printer for the AST.
 */

#ifndef EXCERPT_PRETTY_HPP
#define EXCERPT_PRETTY_HPP

#include "excerpt/visitors/base.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace excerpt {
  /**
   * @brief Concept to check if a type supports the `<<` operator.
   * @tparam T The type to check.
   */
  template <typename T>
  concept OutputStream =
      requires(T &stream, typename T::char_type c) { stream << c; };

  /**
   * @brief The PrettyPrinter class is responsible for printing the AST in
   * readable form.
   */
  class PrettyPrinter : public ASTVisitor {
  public:
    /**
     * @brief Construct a new PrettyPrinter object.
     */
    PrettyPrinter() : stream(std::cout) {}

    /**
     * @brief Construct a new PrettyPrinter object.
     * @tparam Stream The output stream type.
     * @param stream The output stream.
     */
    template <OutputStream Stream>
    PrettyPrinter(Stream &stream) : stream(stream) {}

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
     * @brief Visit a VariableNode node.
     * @param node The VariableNode node to visit.
     */
    auto visit(const VariableNode &node) -> RetT override;

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
     * indentation level in the PrettyPrinter.
     */
    class IndentGuard {
    public:
      /**
       * @brief Construct a new IndentGuard object.
       * @param printer The PrettyPrinter object.
       */
      explicit IndentGuard(PrettyPrinter &printer) : printer(printer) {
        printer.indentLevel++;
      }

      /**
       * @brief Destroy the IndentGuard object.
       */
      ~IndentGuard() { printer.indentLevel--; }

    private:
      PrettyPrinter &printer; /**< The PrettyPrinter object. */
    };

    // Allow the IndentGuard class to access the private members of the
    friend class IndentGuard;

    int indentLevel = 0;  /**< The current indentation level. */
    std::ostream &stream; /**< The output stream. */
  };
} // namespace excerpt

#endif // EXCERPT_PRETTY_HPP
