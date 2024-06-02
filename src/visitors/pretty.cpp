/**
 * @file pretty.cpp
 * @brief Implementation of the PrettyPrinter class.
 */

#include "excerpt/visitors/pretty.hpp"

namespace excerpt {
  void PrettyPrinter::visit(ProgramNode &node) {
    printIndent() << "Program:\n";
    IndentGuard guard(*this);

    for (auto &stmtVariant : node.getBody()) {
      std::visit([this](auto &stmtPtr) { stmtPtr->accept(*this); },
                 stmtVariant);
    }
  }

  void PrettyPrinter::visit(LiteralNode &node) {
    printIndent() << "Literal: " << node.getValue() << '\n';
  }

  void PrettyPrinter::visit(VarDeclNode &node) {
    printIndent() << "VarDecl: " << node.getName() << " : "
                  << node.getTypeInfo().name << '\n';

    IndentGuard guard(*this);
    std::visit([this](auto &value) { value->accept(*this); }, node.getValue());
  }

  void PrettyPrinter::visit(VariableNode &node) {
    printIndent() << "Variable: " << node.getName() << '\n';
  }

  void PrettyPrinter::visit(BinaryNode &node) {
    printIndent() << "Binary: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    std::visit([this](auto &lhsPtr) { lhsPtr->accept(*this); }, node.getLHS());
    std::visit([this](auto &rhsPtr) { rhsPtr->accept(*this); }, node.getRHS());
  }

  void PrettyPrinter::visit(UnaryNode &node) {
    printIndent() << "Unary: " << node.getOp() << '\n';
    IndentGuard guard(*this);

    std::visit([this](auto &operandPtr) { operandPtr->accept(*this); },
               node.getOperand());
  }

  void PrettyPrinter::visit(ProtoNode &node) {
    printIndent() << "Proto: " << node.getName() << '\n';
    IndentGuard guard(*this);

    for (auto &param : node.getParams()) {
      printIndent() << "Arg: " << param.name << " : " << param.typeInfo.name
                    << '\n';
    }

    printIndent() << "Return: " << node.getRetType().name << '\n';
  }

  void PrettyPrinter::visit(BlockNode &node) {
    printIndent() << "Block:\n";
    IndentGuard guard(*this);

    for (auto &stmtVariant : node.getBody()) {
      std::visit([this](auto &stmtPtr) { stmtPtr->accept(*this); },
                 stmtVariant);
    }
  }

  void PrettyPrinter::visit(FuncDeclNode &node) {
    printIndent() << "FuncDecl:\n";
    IndentGuard guard(*this);

    std::visit([this](auto &proto) { proto->accept(*this); }, node.getProto());
    std::visit([this](auto &body) { body->accept(*this); }, node.getBody());
  }

  void PrettyPrinter::visit(CallNode &node) {
    printIndent() << "Call:\n";
    IndentGuard guard(*this);

    std::visit([this](auto &callee) { callee->accept(*this); },
               node.getCallee());

    printIndent() << "Args:\n";
    IndentGuard argsGuard(*this);

    for (auto &argVariant : node.getArgs()) {
      std::visit([this](auto &argPtr) { argPtr->accept(*this); }, argVariant);
    }
  }

  void PrettyPrinter::visit(ReturnNode &node) {
    printIndent() << "Return:\n";
    IndentGuard guard(*this);

    std::visit([this](auto &value) { value->accept(*this); }, node.getValue());
  }
} // namespace excerpt
