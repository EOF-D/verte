/**
 * @file ast.cpp
 * @brief Implements the visitor pattern for the AST.
 */

#include "excerpt/parser/ast.hpp"
#include "excerpt/visitors/base.hpp"

namespace excerpt {
  // clang-format off
  void ProgramAST::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void LiteralNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void VarDeclNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }
  
  void IdentNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void BinaryNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }
  
  void UnaryNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void ProtoNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void BlockNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void FuncDeclNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void ReturnNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }

  void CallNode::accept(ASTVisitor &visitor) {
    visitor.visit(*this);
  }
  // clang-format on
} // namespace excerpt
