/**
 * @file ast.cpp
 * @brief Implementations of the `accept` methods for AST nodes.
 */

#include "excerpt/parser/ast.hpp"
#include "excerpt/visitors/base.hpp"

namespace excerpt {
  auto ProgramNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto LiteralNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto VarDeclNode::accept(ASTVisitor &visitor) const -> RetT {
    return visitor.visit(*this);
  }

  auto VariableNode::accept(ASTVisitor &visitor) const -> RetT {
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
} // namespace excerpt
