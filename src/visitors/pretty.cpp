/**
 * @file pretty.cpp
 * @brief Implementation of the pretty printer for the AST.
 */

#include "excerpt/visitors/pretty.hpp"

namespace excerpt {
  void PrettyPrinter::visit(ProgramAST &node) {
    printIndent() << "Program:\n";

    IndentGuard guard(*this);
    for (auto &stmt : node.getBody()) {
      stmt->accept(*this);
    }
  }

  void PrettyPrinter::visit(LiteralNode &node) {
    printIndent() << "Literal: " << node.getRaw() << '\n';
  }

  void PrettyPrinter::visit(VarDeclNode &node) {
    printIndent() << "VarDecl: " << node.getName() << " : "
                  << typeToString(node.getTypeInfo()) << '\n';

    IndentGuard guard(*this);
    node.getValue()->accept(*this);
  }

  void PrettyPrinter::visit(IdentNode &node) {
    printIndent() << "Ident: " << node.getName() << '\n';
  }

  void PrettyPrinter::visit(BinaryNode &node) {
    printIndent() << "Binary: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    node.getLHS()->accept(*this);
    node.getRHS()->accept(*this);
  }

  void PrettyPrinter::visit(UnaryNode &node) {
    printIndent() << "Unary: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    node.getExpr()->accept(*this);
  }

  void PrettyPrinter::visit(ProtoNode &node) {
    printIndent() << "Proto: " << node.getName() << '\n';

    IndentGuard guard(*this);
    for (auto &arg : node.getParams())
      printIndent() << "Arg: " << arg.name << " : " << typeToString(arg.type)
                    << '\n';

    printIndent() << "Return: " << typeToString(node.getRet()) << '\n';
  }

  void PrettyPrinter::visit(BlockNode &node) {
    printIndent() << "Block:\n";

    IndentGuard guard(*this);
    for (auto &stmt : node.getStmts()) {
      stmt->accept(*this);
    }
  }

  void PrettyPrinter::visit(FuncDeclNode &node) {
    printIndent() << "FuncDecl:\n";
    IndentGuard guard(*this);

    node.getProto().accept(*this);
    node.getBody().accept(*this);
  }

  void PrettyPrinter::visit(ReturnNode &node) {
    printIndent() << "Return:\n";
    IndentGuard guard(*this);

    node.getExpr()->accept(*this);
  }

  void PrettyPrinter::visit(CallNode &node) {
    printIndent() << "Call:\n";
    IndentGuard guard(*this);

    node.getCallee()->accept(*this);
    printIndent() << "Args:\n";

    IndentGuard argsGuard(*this);
    for (auto &arg : node.getArgs()) {
      arg->accept(*this);
    }
  }
} // namespace excerpt
