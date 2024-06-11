/**
 * @brief AST node definitions.
 * @file ast.hpp
 */

#ifndef VERTE_FRONTEND_PARSER_AST_HPP
#define VERTE_FRONTEND_PARSER_AST_HPP

#include "verte/types.hpp"

#include <memory>

// Forward declaration.
namespace verte::visitors {
  class ASTVisitor;
}

/**
 * @namespace verte::nodes
 * @brief AST node classes and related functions & classes.
 */
namespace verte::nodes {
  using namespace verte::types;
  using namespace verte::visitors;

  // Forward declaration.
  class ASTNode;
  class ProtoNode;
  class BlockNode;
  class VariableNode;
  class IfNode;

  /**
   * @typedef NodePtr
   * @brief Unique pointer to an AST node.
   */
  using NodePtr = std::unique_ptr<ASTNode>;

  /**
   * @typedef ProtoPtr
   * @brief Unique pointer to a prototype node.
   */
  using ProtoPtr = std::unique_ptr<ProtoNode>;

  /**
   * @typedef BlockPtr
   * @brief Unique pointer to a block node.
   */
  using BlockPtr = std::unique_ptr<BlockNode>;

  /**
   * @typedef VariablePtr
   * @brief Unique pointer to a variable node.
   */
  using VariablePtr = std::unique_ptr<VariableNode>;

  /**
   * @typedef ifNodePtr
   * @brief Unique pointer to an if node.
   */
  using IfNodePtr = std::unique_ptr<IfNode>;
} // namespace verte::nodes

/**
 * @namespace verte::nodes
 * @brief AST node classes and related functions & classes.
 */
namespace verte::nodes {
  /**
   * @class ASTNode
   * @brief Base class for all AST nodes.
   */
  class ASTNode {
  public:
    /**
     * @brief Default destructor.
     */
    virtual ~ASTNode() = default;

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     * @return The return type of the visitor.
     */
    virtual auto accept(ASTVisitor &visitor) const -> types::RetT = 0;
  };

  /**
   * @class ProgramNode
   * @brief Program node (root).
   */
  class ProgramNode : public ASTNode {
  public:
    /**
     * @brief Default constructor.
     */
    ProgramNode() noexcept = default;

    /**
     * @brief Construct a new ProgramNode.
     * @param body Program body.
     */
    explicit ProgramNode(std::vector<NodePtr> body) noexcept
        : body(std::move(body)) {}

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
    auto accept(ASTVisitor &visitor) const -> types::RetT override;

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
     * @param type Type information of the literal.
     * @param value Value of the literal.
     */
    LiteralNode(const std::string value, TypeInfo type) noexcept
        : value(std::move(value)), type(type) {}

    /**
     * @brief Get the value of the literal.
     * @return Value of the literal.
     */
    const std::string &getValue() const { return value; }

    /**
     * @brief Get the type information of the literal.
     * @return Type information of the literal.
     */
    const TypeInfo &getType() const { return type; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> types::RetT override;

  private:
    std::string value; /**< Value of the literal. */
    TypeInfo type;     /**< Type information. */
  };

  /**
   * @brief Variable declaration node.
   */
  class VarDeclNode : public ASTNode {
  public:
    /**
     * @brief Construct a new VarDeclNode
     * @param name Name of the variable.
     * @param type Type information of the variable.
     * @param value Value of the variable.
     * @param isConst Whether the variable is constant. Default is false.
     */
    VarDeclNode(std::string name, TypeInfo type, NodePtr value,
                bool isConst = false) noexcept
        : name(std::move(name)), type(type), value(std::move(value)),
          isConst(isConst) {}

    /**
     * @brief Get the name of the variable.
     * @return Name of the variable.
     */
    const std::string &getName() const { return name; }

    /**
     * @brief Get the type information of the variable.
     * @return Type information of the variable.
     */
    const TypeInfo &getType() const { return type; }

    /**
     * @brief Get the value of the variable.
     * @return Value of the variable.
     */
    const NodePtr &getValue() const { return value; }

    /**
     * @brief Check if the variable is constant.
     * @return Whether the variable is constant.
     */
    const bool isConstant() const { return isConst; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> types::RetT override;

  private:
    std::string name; /**< Name of the variable. */
    TypeInfo type;    /**< Type information. */
    NodePtr value;    /**< Value of the variable. */
    bool isConst;     /**< Whether the variable is constant. */
  };

  /**
   * @class AssignNode
   * @brief Assignment node.
   */
  class AssignNode : public ASTNode {
  public:
    /**
     * @brief Construct a new AssignNode.
     * @param name Name of the variable.
     * @param value Value to assign.
     */
    AssignNode(std::string name, NodePtr value) noexcept
        : name(std::move(name)), value(std::move(value)) {}

    /**
     * @brief Get the name of the variable.
     * @return Name of the variable.
     */
    const std::string &getName() const { return name; }

    /**
     * @brief Get the value to assign.
     * @return Value to assign.
     */
    const NodePtr &getValue() const { return value; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    std::string name; /**< Name of the variable. */
    NodePtr value;    /**< Value to assign. */
  };

  /**
   * @class VariableNode
   * @brief Variable node.
   */
  class VariableNode : public ASTNode {
  public:
    /**
     * @brief Construct a new VariableNode.
     * @param name Name of the variable.
     */
    VariableNode(std::string name) noexcept : name(std::move(name)) {}

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
   * @class IfNode
   * @brief If statement node.
   */
  class IfNode : public ASTNode {
  public:
    /**
     * @brief Construct a new IfNode.
     * @param cond Condition of the if statement.
     * @param body Body of the if statement.
     * @param elseBody Else body of the if statement.
     */
    IfNode(NodePtr cond, BlockPtr body) noexcept
        : cond(std::move(cond)), block(std::move(body)) {}

    /**
     * @brief Get the condition of the if statement.
     * @return The condition of the if statement.
     */
    const NodePtr &getCond() const { return cond; }

    /**
     * @brief Get the body of the if statement.
     * @return The body of the if statement.
     */
    const BlockPtr &getBlock() const { return block; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    NodePtr cond;   /**< Condition of the if statement. */
    BlockPtr block; /**< Then block of the if statement. */
  };

  /**
   * @class IfElseNode
   * @brief If-else statement node.
   */
  class IfElseNode : public ASTNode {
  public:
    /**
     * @brief Construct a new IfElseNode.
     * @param ifNode If statement node.
     * @param elseBlock Else block node.
     */
    IfElseNode(IfNodePtr ifNode, BlockPtr elseBlock) noexcept
        : ifNode(std::move(ifNode)), elseBlock(std::move(elseBlock)) {}

    /**
     * @brief Get the if statement of the if-else statement.
     * @return The if statement of the if-else statement.
     */
    const IfNodePtr &getIfNode() const { return ifNode; }

    /**
     * @brief Get the else block of the if-else statement.
     * @return The else block of the if-else statement.
     */
    const BlockPtr &getElseBlock() const { return elseBlock; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    IfNodePtr ifNode;   /**< If statement of the if-else statement. */
    BlockPtr elseBlock; /**< Else block of the if-else statement. */
  };

  /**
   * @class BinaryNode
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
    BinaryNode(NodePtr lhs, NodePtr rhs, std::string op) noexcept
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
   * @class UnaryNode
   * @brief Unary operation node.
   */
  class UnaryNode : public ASTNode {
  public:
    /**
     * @brief Construct a new UnaryNode.
     * @param operand Operand of the unary operation.
     * @param op Operator.
     */
    UnaryNode(NodePtr operand, std::string op) noexcept
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
    ProtoNode(const std::string &name, std::vector<Parameter> params,
              TypeInfo returnType)
        : name(std::move(name)), params(std::move(params)),
          returnType(returnType) {}

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
   * @class BlockNode
   * @brief Block node.
   */
  class BlockNode : public ASTNode {
  public:
    /**
     * @brief Construct a new BlockNode.
     * @param body Body of the block.
     */
    BlockNode(std::vector<NodePtr> body) noexcept : body(std::move(body)) {}

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
   * @class FuncDeclNode
   * @brief Function declaration node.
   */
  class FuncDeclNode : public ASTNode {
  public:
    /**
     * @brief Construct a new FuncDeclNode.
     * @param proto Prototype of the function.
     * @param body Body of the function.
     */
    FuncDeclNode(ProtoPtr proto, BlockPtr body) noexcept
        : proto(std::move(proto)), body(std::move(body)) {}

    /**
     * @brief Get the prototype of the function.
     * @return Prototype of the function.
     */
    const ProtoPtr &getProto() const { return proto; }

    /**
     * @brief Get the body of the function.
     * @return Body of the function.
     */
    const BlockPtr &getBody() const { return body; }

    /**
     * @brief Accept a visitor for the node.
     * @param visitor Visitor to accept.
     * @return The return value of the visitor.
     */
    auto accept(ASTVisitor &visitor) const -> RetT override;

  private:
    ProtoPtr proto; /**< Prototype of the function. */
    BlockPtr body;  /**< Body of the function. */
  };

  /**
   * @class CallNode
   * @brief Function call node.
   */
  class CallNode : public ASTNode {
  public:
    /**
     * @brief Construct a new CallNode.
     * @param callee Callee of the function call.
     * @param args Arguments of the function call.
     */
    CallNode(VariablePtr callee, std::vector<NodePtr> args) noexcept
        : callee(std::move(callee)), args(std::move(args)) {}

    /**
     * @brief Get the callee of the call expression.
     * @return The callee of the call expression.
     */
    const VariablePtr &getCallee() const { return callee; }

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
    VariablePtr callee;        /**< The callee of the call expression. */
    std::vector<NodePtr> args; /**< The args to call with. */
  };

  /**
   * @class ReturnNode
   * @brief Return statement node.
   */
  class ReturnNode : public ASTNode {
  public:
    /**
     * @brief Construct a new ReturnNode.
     * @param value The value to return.
     */
    ReturnNode(NodePtr value) noexcept : value(std::move(value)) {}

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
} // namespace verte::nodes

#endif // VERTE_FRONTEND_PARSER_AST_HPP
