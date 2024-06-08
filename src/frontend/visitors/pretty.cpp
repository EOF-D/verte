/**
 * @brief Pretty printer implementation.
 * @file pretty.cpp
 */

#include "verte/frontend/visitors/pretty.hpp"

namespace verte::visitors {
  auto PrettyPrinter::visit(const ProgramNode &node) -> RetT {
    printIndent() << "Program:\n";
    IndentGuard guard(*this);

    for (const auto &stmt : node.getBody()) {
      stmt->accept(*this);
    }

    return {};
  }

  auto PrettyPrinter::visit(const LiteralNode &node) -> RetT {
    printIndent() << "Literal: " << node.getValue() << '\n';
    return {};
  }

  auto PrettyPrinter::visit(const VarDeclNode &node) -> RetT {
    printIndent() << "VarDecl: " << node.getName() << " : "
                  << node.getType().name << '\n';

    IndentGuard guard(*this);
    node.getValue()->accept(*this);
    printIndent() << "Constant: " << (node.isConstant() ? "true" : "false")
                  << '\n';

    return {};
  }

  auto PrettyPrinter::visit(const AssignNode &node) -> RetT {
    printIndent() << "Assign:\n";

    IndentGuard guard(*this);
    printIndent() << "Variable: " << node.getName() << '\n';
    node.getValue()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const VariableNode &node) -> RetT {
    printIndent() << "Variable: " << node.getName() << '\n';
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
      printIndent() << "Arg: " << param.name << " : " << param.type.name
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
} // namespace verte::visitors
