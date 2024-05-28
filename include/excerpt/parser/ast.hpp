/**
 * @file ast.hpp
 * @brief This file contains the AST nodes for the parser.
 */

#ifndef EXCERPT_AST_HPP
#define EXCERPT_AST_HPP

#include "excerpt/types.hpp"
#include "llvm/IR/Value.h"

#include <concepts>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace excerpt {
  // Forward declaration.
  class ASTVisitor;
  class ASTNode;

  // Type aliases.
  using RetT = std::variant<std::string, TypeInfo, llvm::Value *>;
  using NodePtr = std::unique_ptr<ASTNode>;

  /**
   * @brief Base class for AST nodes.
   */
  class ASTNode {
  public:
    /**
     * @brief Default destructor.
     */
    virtual ~ASTNode() = default;

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * \note
     * Pure virtual method, subclasses should implement.
     */
    virtual auto accept(ASTVisitor &visitor) const -> RetT = 0;
  };

  /**
   * @brief AST node concept.
   * @tparam T Type of the node.
   */
  template <typename T>
  concept ASTNodeType = std::derived_from<T, ASTNode>;

  /**
   * @brief Program node (root).
   */
  class ProgramNode : public ASTNode {
  public:
    /**
     * @brief Default constructor.
     */
    ProgramNode() = default;

    /**
     * @brief Construct a new ProgramNode.
     * @param body Program body.
     */
    explicit ProgramNode(std::vector<NodePtr> body) : body(std::move(body)) {}

    /**
     * @brief Get the program body.
     * @return Program body.
     */
    const std::vector<NodePtr> &getBody() const { return body; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::vector<NodePtr> body; /**< Program body. */
  };

  /**
   * @brief Literal node.
   */
  class LiteralNode : public ASTNode {
  public:
    /**
     * @brief Construct a new LiteralNode.
     * @param typeInfo Type information of the literal.
     * @param value Value of the literal.
     */
    LiteralNode(std::string value, TypeInfo typeInfo)
        : value(std::move(value)), typeInfo(typeInfo) {}

    /**
     * @brief Get the value of the literal.
     * @return Value of the literal.
     */
    const std::string &getValue() const { return value; }

    /**
     * @brief Get the type information of the literal.
     * @return Type information of the literal.
     */
    const TypeInfo &getTypeInfo() const { return typeInfo; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::string value; /**< Value of the literal. */
    TypeInfo typeInfo; /**< Type information. */
  };

  /**
   * @brief Variable declaration node.
   */
  class VarDeclNode : public ASTNode {
  public:
    /**
     * @brief Construct a new VarDeclNode
     * @param name Name of the variable.
     * @param typeInfo Type information of the variable.
     * @param value Value of the variable.
     */
    VarDeclNode(std::string name, TypeInfo typeInfo, NodePtr value)
        : name(std::move(name)), typeInfo(typeInfo), value(std::move(value)) {}

    /**
     * @brief Get the name of the variable.
     * @return Name of the variable.
     */
    const std::string &getName() const { return name; }

    /**
     * @brief Get the type information of the variable.
     * @return Type information of the variable.
     */
    const TypeInfo &getTypeInfo() const { return typeInfo; }

    /**
     * @brief Get the value of the variable.
     * @return Value of the variable.
     */
    const NodePtr &getValue() const { return value; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::string name;  /**< Name of the variable. */
    TypeInfo typeInfo; /**< Type information. */
    NodePtr value;     /**< Value of the variable. */
  };

  /**
   * @brief Variable node.
   */
  class VariableNode : public ASTNode {
  public:
    /**
     * @brief Construct a new VariableNode.
     * @param name Name of the variable.
     */
    VariableNode(std::string name) : name(std::move(name)) {}

    /**
     * @brief Get the name of the variable.
     * @return Name of the variable.
     */
    const std::string &getName() const { return name; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::string name; /**< Name of the variable. */
  };

  /**
   * @brief Binary operation node.
   */
  class BinaryNode : public ASTNode {
  public:
    /**
     * @brief Construct a new BinaryNode.
     * @param lhs Left-hand side of the binary operation.
     * @param rhs Right-hand side of the binary operation.
     * @param op Operator.
     */
    BinaryNode(NodePtr lhs, NodePtr rhs, std::string op)
        : lhs(std::move(lhs)), rhs(std::move(rhs)), op(std::move(op)) {}

    /**
     * @brief Get the left-hand side expression.
     * @return The left-hand side expression.
     */
    const NodePtr &getLHS() const { return lhs; }

    /**
     * @brief Get the right-hand side expression.
     * @return The right-hand side expression.
     */
    const NodePtr &getRHS() const { return rhs; }

    /**
     * @brief Get the operator.
     * @return The operator.
     */
    const std::string &getOp() const { return op; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    NodePtr lhs;    /**< Left-hand side. */
    NodePtr rhs;    /**< Right-hand side. */
    std::string op; /**< Operator. */
  };

  /**
   * @brief Unary operation node.
   */
  class UnaryNode : public ASTNode {
  public:
    /**
     * @brief Construct a new UnaryNode.
     * @param operand Operand of the unary operation.
     * @param op Operator.
     */
    UnaryNode(NodePtr operand, std::string op)
        : operand(std::move(operand)), op(std::move(op)) {}

    /**
     * @brief Get the operand of the unary operation.
     * @return The operand of the unary operation.
     */
    const NodePtr &getOperand() const { return operand; }

    /**
     * @brief Get the operator.
     * @return The operator.
     */
    const std::string &getOp() const { return op; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    NodePtr operand; /**< Operand. */
    std::string op;  /**< Operator. */
  };

  /**
   * @brief Prototype node.
   */
  class ProtoNode : public ASTNode {
  public:
    /**
     * @brief Construct a new ProtoNode.
     * @param name Name of the function.
     * @param args Arguments of the function.
     * @param returnType Return type of the function.
     */
    // clang-format off
    ProtoNode(const std::string &name, std::vector<Parameter> params, TypeInfo returnType)
        : name(std::move(name)), params(std::move(params)), returnType(returnType) {}
    // clang-format on

    /**
     * @brief Get the name of the function.
     * @return Name of the function.
     */
    const std::string &getName() const { return name; }

    /**
     * @brief Get the parameters of the function.
     * @return Parameters of the function.
     */
    const std::vector<Parameter> &getParams() const { return params; }

    /**
     * @brief Get the return type of the function.
     * @return Return type of the function.
     */
    const TypeInfo &getRetType() const { return returnType; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::string name;              /**< Name of the function. */
    std::vector<Parameter> params; /**< Arguments of the function. */
    TypeInfo returnType;           /**< Return type. */
  };

  /**
   * @brief Block node.
   */
  class BlockNode : public ASTNode {
  public:
    /**
     * @brief Construct a new BlockNode.
     * @param body Body of the block.
     */
    BlockNode(std::vector<NodePtr> body) : body(std::move(body)) {}

    /**
     * @brief Get the body of the block.
     * @return Body of the block.
     */
    const std::vector<NodePtr> &getBody() const { return body; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::vector<NodePtr> body; /**< Body of the block. */
  };

  /**
   * @brief Function declaration node.
   */
  class FuncDeclNode : public ASTNode {
  public:
    /**
     * @brief Construct a new FuncDeclNode.
     * @param proto Prototype of the function.
     * @param body Body of the function.
     */
    FuncDeclNode(NodePtr proto, NodePtr body)
        : proto(std::move(proto)), body(std::move(body)) {}

    /**
     * @brief Get the prototype of the function.
     * @return Prototype of the function.
     */
    const NodePtr &getProto() const { return proto; }

    /**
     * @brief Get the body of the function.
     * @return Body of the function.
     */
    const NodePtr &getBody() const { return body; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    NodePtr proto; /**< Prototype. */
    NodePtr body;  /**< Body. */
  };

  /**
   * @brief Function call node.
   */
  class CallNode : public ASTNode {
  public:
    /**
     * @brief Construct a new CallNode.
     * @param callee Callee of the function call.
     * @param args Arguments of the function call.
     */
    CallNode(NodePtr callee, std::vector<NodePtr> args)
        : callee(std::move(callee)), args(std::move(args)) {}

    /**
     * @brief Get the callee of the call expression.
     * @return The callee of the call expression.
     */
    const NodePtr &getCallee() const { return callee; }

    /**
     * @brief Get the arguments of the call expression.
     * @return The arguments of the call expression.
     */
    const std::vector<NodePtr> &getArgs() const { return args; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    NodePtr callee;            /**< The callee of the call expression. */
    std::vector<NodePtr> args; /**< The args to call with. */
  };

  /**
   * @brief Return statement node.
   */
  class ReturnNode : public ASTNode {
  public:
    /**
     * @brief Construct a new ReturnNode.
     * @param value The value to return.
     */
    ReturnNode(NodePtr value) : value(std::move(value)) {}

    /**
     * @brief Get the value to return.
     * @return The value to return.
     */
    const NodePtr &getValue() const { return value; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    NodePtr value; /**< The value to return. */
  };
} // namespace excerpt

#endif // EXCERPT_AST_HPP
