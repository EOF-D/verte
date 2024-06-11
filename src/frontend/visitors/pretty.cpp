/**
 * @brief Pretty printer implementation.
 * @file pretty.cpp
 */

#include "verte/frontend/visitors/pretty.hpp"
#include "verte/frontend/parser/ast.hpp"
#include <llvm/Support/raw_ostream.h>

namespace verte::visitors {
  auto PrettyPrinter::visit(const ProgramNode &node) -> RetT {
    printIndent() << "Program Node:\n";
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
    printIndent() << "VarDecl Node: " << node.getName() << " : "
                  << node.getType().name << '\n';

    IndentGuard guard(*this);
    node.getValue()->accept(*this);
    printIndent() << "Constant: " << (node.isConstant() ? "true" : "false")
                  << '\n';

    return {};
  }

  auto PrettyPrinter::visit(const AssignNode &node) -> RetT {
    printIndent() << "Assign Node:\n";

    IndentGuard guard(*this);
    printIndent() << "Variable: " << node.getName() << '\n';
    node.getValue()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const VariableNode &node) -> RetT {
    printIndent() << "Variable: " << node.getName() << '\n';
    return {};
  }

  auto PrettyPrinter::visit(const IfNode &node) -> RetT {
    printIndent() << "If Node:\n";

    IndentGuard guard(*this);
    printIndent() << "Condition:\n";
    node.getCond()->accept(*this);

    printIndent() << "Body:\n";
    node.getBlock()->accept(*this);

    return {};
  }

  auto PrettyPrinter::visit(const IfElseNode &node) -> RetT {
    printIndent() << "IfElse Node:\n";

    IndentGuard guard(*this);
    printIndent() << "IfNode:\n";
    node.getIfNode()->accept(*this);

    printIndent() << "Else:\n";
    node.getElseBlock()->accept(*this);

    return {};
  }

  auto PrettyPrinter::visit(const BinaryNode &node) -> RetT {
    printIndent() << "Binary Node: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    node.getLHS()->accept(*this);
    node.getRHS()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const UnaryNode &node) -> RetT {
    printIndent() << "Unary Node: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    node.getOperand()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const ProtoNode &node) -> RetT {
    printIndent() << "Proto Node: " << node.getName() << '\n';
    IndentGuard guard(*this);

    for (const auto &param : node.getParams()) {
      printIndent() << "Arg: " << param.name << " : " << param.type.name
                    << '\n';
    }

    printIndent() << "Return Node: " << node.getRetType().name << '\n';
    return {};
  }

  auto PrettyPrinter::visit(const BlockNode &node) -> RetT {
    printIndent() << "Block Node:\n";
    IndentGuard guard(*this);

    for (const auto &stmt : node.getBody()) {
      stmt->accept(*this);
    }

    return {};
  }

  auto PrettyPrinter::visit(const FuncDeclNode &node) -> RetT {
    printIndent() << "FuncDecl Node:\n";
    IndentGuard guard(*this);

    node.getProto()->accept(*this);
    node.getBody()->accept(*this);
    return {};
  }

  auto PrettyPrinter::visit(const CallNode &node) -> RetT {
    printIndent() << "Call Node:\n";
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
    printIndent() << "Return Node:\n";
    IndentGuard guard(*this);

    node.getValue()->accept(*this);
    return {};
  }
} // namespace verte::visitors
