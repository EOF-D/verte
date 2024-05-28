/**
 * @file pretty.cpp
 * @brief Implementation of the PrettyPrinter class.
 */

#include "excerpt/visitors/pretty.hpp"

namespace excerpt {

  auto PrettyPrinter::visit(const ProgramNode &node) -> RetT {
    printIndent() << "Program:\n";
    IndentGuard guard(*this);

    for (const auto &stmt : node.getBody()) {
      stmt->accept(*this);
    }

    return {};
  }

  auto PrettyPrinter::visit(const LiteralNode &node) -> RetT {
    printIndent() << "Literal: " << node.getValue() << " : "
                  << node.getTypeInfo().name << '\n';

    return {};
  }

  auto PrettyPrinter::visit(const VarDeclNode &node) -> RetT {
    printIndent() << "VarDecl: " << node.getName() << " : "
                  << node.getTypeInfo().name << '\n';

    IndentGuard guard(*this);
    node.getValue()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const VariableNode &node) -> RetT {
    printIndent() << "Ident: " << node.getName() << '\n';
    return {};
  }

  auto PrettyPrinter::visit(const BinaryNode &node) -> RetT {
    printIndent() << "Binary: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    node.getLHS()->accept(*this);
    node.getRHS()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const UnaryNode &node) -> RetT {
    printIndent() << "Unary: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    node.getOperand()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const ProtoNode &node) -> RetT {
    printIndent() << "Proto: " << node.getName() << '\n';
    IndentGuard guard(*this);

    for (const auto &param : node.getParams()) {
      printIndent() << "Arg: " << param.name << " : " << param.typeInfo.name
                    << '\n';
    }

    printIndent() << "Return: " << node.getRetType().name << '\n';
    return {};
  }

  auto PrettyPrinter::visit(const BlockNode &node) -> RetT {
    printIndent() << "Block:\n";
    IndentGuard guard(*this);

    for (const auto &stmt : node.getBody()) {
      stmt->accept(*this);
    }

    return {};
  }

  auto PrettyPrinter::visit(const FuncDeclNode &node) -> RetT {
    printIndent() << "FuncDecl:\n";
    IndentGuard guard(*this);

    node.getProto()->accept(*this);
    node.getBody()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const CallNode &node) -> RetT {
    printIndent() << "Call:\n";
    IndentGuard guard(*this);

    node.getCallee()->accept(*this);
    printIndent() << "Args:\n";
    IndentGuard argsGuard(*this);

    for (const auto &arg : node.getArgs()) {
      arg->accept(*this);
    }

    return {};
  }

  auto PrettyPrinter::visit(const ReturnNode &node) -> RetT {
    printIndent() << "Return:\n";
    IndentGuard guard(*this);

    node.getValue()->accept(*this);
    return {};
  }
} // namespace excerpt
