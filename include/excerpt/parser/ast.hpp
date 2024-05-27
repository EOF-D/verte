/**
 * @file ast.hpp
 * @brief AST nodes used for the parser.
 */

#ifndef EXCERPT_AST_HPP
#define EXCERPT_AST_HPP

#include "excerpt/errors.hpp"

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/IR/Value.h>

#include <format>
#include <memory>
#include <variant>

namespace excerpt {
  // Forward declarations.
  class ASTNode;
  class BlockNode;
  class ProtoNode;

  class ASTVisitor;
  class TypeInfo;
  class Parameter;

  // Type aliases.
  using NodePtr = std::unique_ptr<ASTNode>;
  using BlockPtr = std::unique_ptr<BlockNode>;
  using ProtoPtr = std::unique_ptr<ProtoNode>;

  using LiteralValue = std::variant<llvm::APSInt, llvm::APFloat, std::string>;
  using Params = std::vector<Parameter>;

  /**
   * @brief Simple struct to represent type information.
   */
  struct TypeInfo {
    /**
     * @brief Enum to represent the type of the variable.
     */
    enum class Type : uint8_t {
      INTEGER, /**< Integer type. */
      FLOAT,   /**< Float type. */
      STRING,  /**< String type. */
      UNKNOWN  /**< Unknown type. */
    } type{Type::UNKNOWN};

    bool isConstant; /**< Whether the variable is a constant. */

    /**
     * @brief Construct a new TypeInfo object.
     * @param type The type of the variable.
     * @param isConstant Whether the variable is a constant.
     */
    TypeInfo(Type type, bool isConstant) : type(type), isConstant(isConstant) {}

    /**
     * @brief Convert a string to a type.
     * @param type The type to convert.
     * @return The converted type.
     */
    static Type toEnum(const std::string &type) {
      if (type == "int")
        return Type::INTEGER;

      if (type == "float")
        return Type::FLOAT;

      if (type == "string")
        return Type::STRING;

      return Type::UNKNOWN;
    }
  };

  /**
   * @brief Represents a parameter in a function declaration.
   */
  struct Parameter {
    std::string name; /**< The name of the parameter. */
    TypeInfo type;    /**< The type of the parameter. */

    /**
     * @brief Construct a new Parameter object.
     * @param name The name of the parameter.
     * @param type The type of the parameter.
     */
    Parameter(const std::string &name, const TypeInfo &type)
        : name(name), type(type) {}
  };

  /**
   * @brief Abstract syntax tree (AST) node.
   */
  class ASTNode {
  public:
    enum class Type : uint8_t {
      PROGRAM,   /**< Program (root) node. */
      LITERAL,   /**< Literal node. */
      VAR_DECL,  /**< Variable declaration node. */
      IDENT,     /**< Identifier node. */
      BINARY,    /**< Binary operation node. */
      UNARY,     /**< Unary operation node. */
      PROTO,     /**< Function prototype node. */
      BLOCK,     /**< Block node. */
      RETURN,    /**< Return statement node. */
      FUNC_DECL, /**< Function declaration node. */
      FUNC_CALL  /**< Function call node. */
    };

    /**
     * @brief Construct a new ASTNode object.
     * @param type The type of the node.
     */
    explicit ASTNode(Type type) : type(type) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    virtual void accept(ASTVisitor &visitor) = 0;

    /**
     * @brief Get the type of the node.
     * @return The type of the node.
     */
    inline Type getType() const { return type; }

  private:
    Type type; /**< The type of the node. */
  };

  /**
   * @brief Program (root) AST node.
   */
  class ProgramAST : public ASTNode {
  public:
    /**
     * @brief Construt a new ProgramAST object.
     * @param body The body of the program.
     */
    ProgramAST(std::vector<NodePtr> body)
        : ASTNode(ASTNode::Type::PROGRAM), body(std::move(body)) {}

    /**
     * @brief Destroy the ProgramAST object.
     */
    ~ProgramAST() {
      for (auto &node : body)
        node.reset();

      body.clear();
    }

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the body of the program.
     * @return The body of the program.
     */
    inline const std::vector<NodePtr> &getBody() const { return body; }

  private:
    std::vector<NodePtr> body; /**< The body of the program. */
  };

  /**
   * @brief Literal AST node.
   */
  class LiteralNode : public ASTNode {
  public:
    /**
     * @brief Construct a new LiteralNode object.
     * @param value The literal value.
     * @param type The type information.
     */
    // clang-format off
    LiteralNode(const std::string &value, const TypeInfo &type) 
      : ASTNode(ASTNode::Type::LITERAL), raw(value), type_info(type) {
        // This isn't done in the initializer list to stop toValue from potentially erroring.
        this->value = toValue(raw);
    }
    // clang-format on

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the raw value.
     * @return The raw value.
     */
    inline const std::string &getRaw() const { return raw; }

    /**
     * @brief Get the literal value.
     * @return The literal value.
     */
    inline const LiteralValue &getValue() const { return value; }
    /**
     * @brief Get the type information.
     * @return The type information.
     */
    inline const TypeInfo &getTypeInfo() const { return type_info; }

  private:
    /**
     * @brief Convert a string value to a LiteralValue.
     * @param value The value to convert.
     * @return The LiteralValue.
     */
    LiteralValue toValue(const std::string &value) {
      // TODO: Implement more types.
      switch (type_info.type) {
        case TypeInfo::Type::INTEGER: {
          return llvm::APSInt(llvm::APInt(32, std::stoi(value)));
        }

        case TypeInfo::Type::FLOAT:
          return llvm::APFloat(std::stod(value));

        case TypeInfo::Type::STRING:
          return value;

        default:
          throw InternalError("Unknown type");
      }
    }

    LiteralValue value; /**< The literal value. */
    TypeInfo type_info; /**< The type information. */
    std::string raw;    /**< The raw value. */
  };

  /**
   * @brief Variable declaration AST node. (e.g., foo: int = 1;)
   */
  class VarDeclNode : public ASTNode {
  public:
    /**
     * @brief Construct a new VarDeclNode object.
     * @param name The variable name.
     * @param type The type information.
     */
    VarDeclNode(const std::string &name, const TypeInfo &type, NodePtr value)
        : ASTNode(ASTNode::Type::VAR_DECL), name(name), type_info(type),
          value(std::move(value)) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the variable name.
     * @return The variable name.
     */
    inline const std::string &getName() const { return name; }

    /**
     * @brief Get the type information.
     * @return The type information.
     */
    inline const TypeInfo &getTypeInfo() const { return type_info; }

    /**
     * @brief Get the value of the variable.
     * @return The value of the variable.
     */
    inline NodePtr &getValue() { return value; }

  private:
    std::string name;   /**< The variable name. */
    TypeInfo type_info; /**< The type information. */
    NodePtr value;      /**< The value of the variable. */
  };

  /**
   * @brief Identifier AST node.
   */
  class IdentNode : public ASTNode {
  public:
    /**
     * @brief Construct a new IdentNode object.
     * @param name The identifier name.
     */
    IdentNode(const std::string &name)
        : ASTNode(ASTNode::Type::IDENT), name(name) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the identifier name.
     * @return The identifier name.
     */
    inline const std::string &getName() const { return name; }

  private:
    std::string name; /**< The identifier name. */
  };

  /**
   * @brief Binary operation AST node. (e.g., +, -, *, /, <, >, ==, etc.)
   */
  class BinaryNode : public ASTNode {
  public:
    /**
     * @brief Construct a new BinaryNode object.
     * @param lhs The left-hand side expression.
     * @param rhs The right-hand side expression.
     * @param op The operator.
     */
    BinaryNode(NodePtr lhs, NodePtr rhs, const std::string &op)
        : ASTNode(ASTNode::Type::BINARY), lhs(std::move(lhs)),
          rhs(std::move(rhs)), op(op) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the left-hand side expression.
     * @return The left-hand side expression.
     */
    inline NodePtr &getLHS() { return lhs; }

    /**
     * @brief Get the right-hand side expression.
     * @return The right-hand side expression.
     */
    inline NodePtr &getRHS() { return rhs; }

    /**
     * @brief Get the operator.
     * @return The operator.
     */
    inline const std::string &getOp() const { return op; }

  private:
    NodePtr lhs;    /**< The left-hand side expression. */
    NodePtr rhs;    /**< The right-hand side expression. */
    std::string op; /**< The operator. */
  };

  /**
   * @brief Unary operation AST node. (e.g., !, -, etc.)
   */
  class UnaryNode : public ASTNode {
  public:
    /**
     * @brief Construct a new UnaryNode object.
     * @param expr The expression.
     * @param op The operator.
     */
    UnaryNode(const std::string op, NodePtr expr)
        : ASTNode(ASTNode::Type::UNARY), op(op), expr(std::move(expr)) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the expression.
     * @return The expression.
     */
    inline NodePtr &getExpr() { return expr; }

    /**
     * @brief Get the operator.
     * @return The operator.
     */
    inline const std::string &getOp() const { return op; }

  private:
    std::string op; /**< The operator. */
    NodePtr expr;   /**< The expression. */
  };

  /**
   * @brief Function prototype AST node.
   */
  class ProtoNode : public ASTNode {
  public:
    /**
     * @brief Construct a new PrototypeExpressionNode object
     * @param name The name of the prototype
     * @param args The args of the prototype.
     * @param ret The return type.
     */
    ProtoNode(const std::string &name, Params &params, TypeInfo ret)
        : ASTNode(ASTNode::Type::PROTO), name(name), args(std::move(params)),
          ret(ret) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the name of the prototype.
     * @return The name of the prototype.
     */
    inline const std::string &getName() const { return name; }

    /**
     * @brief Get the parameters of the prototype.
     * @return The parameters of the prototype.
     */
    inline const Params &getParams() const { return args; }

    /**
     * @brief Get the return type of the prototype.
     * @return The return type of the prototype.
     */
    inline TypeInfo getRet() const { return ret; }

  private:
    std::string name; /**< The name of the prototype. */
    Params args;      /**< The arguments of the prototype. */
    TypeInfo ret;     /**< The return type of the prototype. */
  };

  /**
   * @brief Block AST node.
   */
  class BlockNode : public ASTNode {
  public:
    /**
     * @brief Construct a new BlockNode object.
     * @param stmts The statements in the block.
     */
    BlockNode(std::vector<NodePtr> stmts)
        : ASTNode(ASTNode::Type::BLOCK), stmts(std::move(stmts)) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the statements in the block.
     * @return The statements in the block.
     */
    inline const std::vector<NodePtr> &getStmts() const { return stmts; }

  private:
    std::vector<NodePtr> stmts; /**< The statements in the block. */
  };

  /**
   * @brief Function declaration AST node.
   */
  class FuncDeclNode : public ASTNode {
  public:
    /**
     * @brief Construct a new FuncDeclNode object.
     * @param proto The function prototype.
     * @param body The function body.
     */
    FuncDeclNode(ProtoPtr proto, BlockPtr body)
        : ASTNode(ASTNode::Type::FUNC_DECL), proto(std::move(proto)),
          body(std::move(body)) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the function prototype.
     * @return The function prototype.
     */
    inline ProtoNode &getProto() const { return *proto; }

    /**
     * @brief Get the function body.
     * @return The function body.
     */
    inline BlockNode &getBody() const { return *body; }

  private:
    ProtoPtr proto; /**< The function prototype. */
    BlockPtr body;  /**< The function body. */
  };

  /**
   * @brief Return statement AST node.
   */
  class ReturnNode : public ASTNode {
  public:
    /**
     * @brief Construct a new ReturnNode object.
     * @param expr The expression to return.
     */
    ReturnNode(NodePtr expr)
        : ASTNode(ASTNode::Type::RETURN), expr(std::move(expr)) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the expression to return.
     * @return The expression to return.
     */
    inline NodePtr &getExpr() { return expr; }

  private:
    NodePtr expr; /**< The expression to return. */
  };

  /**
   * @brief Function call AST node.
   */
  class CallNode : public ASTNode {
  public:
    /**
     * @brief Construct a new CallNode object.
     * @param callee The callee.
     * @param args The arguments.
     */
    CallNode(NodePtr callee, std::vector<NodePtr> args)
        : ASTNode(ASTNode::Type::FUNC_CALL), callee(std::move(callee)),
          args(std::move(args)) {}

    /**
     * @brief Accept a visitor.
     * @param visitor The visitor to accept.
     */
    void accept(ASTVisitor &visitor) override;

    /**
     * @brief Get the callee.
     * @return The callee.
     */
    inline NodePtr &getCallee() { return callee; }

    /**
     * @brief Get the arguments.
     * @return The arguments.
     */
    inline const std::vector<NodePtr> &getArgs() const { return args; }

  private:
    NodePtr callee;            /**< The callee. */
    std::vector<NodePtr> args; /**< The arguments. */
  };
} // namespace excerpt

#endif // EXCERPT_AST_HPP
