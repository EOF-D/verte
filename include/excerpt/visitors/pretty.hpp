/**
 * @file pretty.hpp
 * @brief Pretty printer for the AST.
 */

#ifndef EXCERPT_PRETTY_HPP
#define EXCERPT_PRETTY_HPP

#include "excerpt/parser/ast.hpp"
#include "excerpt/visitors/base.hpp"

#include <iostream>
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
    void visit(ProgramAST &node) override;

    /**
     * @brief Visit a LiteralNode node.
     * @param node The LiteralNode node to visit.
     */
    void visit(LiteralNode &node) override;

    /**
     * @brief Visit a VarDeclNode node.
     * @param node The VarDeclNode node to visit.
     */
    void visit(VarDeclNode &node) override;

    /**
     * @brief Visit an IdentNode node.
     * @param node The IdentNode node to visit.
     */
    void visit(IdentNode &node) override;

    /**
     * @brief Visit a BinaryNode node.
     * @param node The BinaryNode node to visit.
     */
    void visit(BinaryNode &node) override;

    /**
     * @brief Visit a UnaryNode node.
     * @param node The UnaryNode node to visit.
     */
    void visit(UnaryNode &node) override;

    /**
     * @brief Visit a ProtoNode node.
     * @param node The ProtoNode node to visit.
     */
    void visit(ProtoNode &node) override;

    /**
     * @brief Visit a BlockNode node.
     * @param node The BlockNode node to visit.
     */
    void visit(BlockNode &node) override;

    /**
     * @brief Visit a FuncDeclNode node.
     * @param node The FuncDeclNode node to visit.
     */
    void visit(FuncDeclNode &node) override;

    /**
     * @brief Visit a ReturnNode node.
     * @param node The ReturnNode node to visit.
     */
    void visit(ReturnNode &node) override;

    /**
     * @brief Visit a CallNode node.
     * @param node The CallNode node to visit.
     */
    void visit(CallNode &node) override;

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
     * @brief Convert a TypeInfo object to a string representation.
     * @param type The TypeInfo object to convert.
     * @return The string representation of the TypeInfo object.
     */
    std::string typeToString(TypeInfo type) {
      switch (type.type) {
        case TypeInfo::Type::INTEGER:
          return "int";

        case TypeInfo::Type::FLOAT:
          return "float";

        case TypeInfo::Type::STRING:
          return "string";

        default:
          return "unknown";
      }
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
