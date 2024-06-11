/**
 * @brief AST node implementations.
 * @file ast.cpp
 */

#include "verte/frontend/parser/ast.hpp"
#include "verte/frontend/visitors/base.hpp"

namespace verte::nodes {
  using namespace verte::visitors;

  auto ProgramNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto LiteralNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto VarDeclNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto AssignNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto VariableNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto IfNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto IfElseNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto BinaryNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto UnaryNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto ProtoNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto BlockNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto FuncDeclNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto CallNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto ReturnNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }
} // namespace verte::nodes
