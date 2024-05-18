/**
 * @brief Abstract syntax tree (AST) nodes.
 * @file ast.hpp
 */

#ifndef EXCERPT_AST_HPP
#define EXCERPT_AST_HPP

#include <format>
#include <memory>
#include <vector>

namespace excerpt {
  // Forward declarations
  class ExprAST;

  /**
   * @brief Expression pointer type.
   */
  using ExprPtr = std::unique_ptr<ExprAST>;

  /**
   * @brief Root AST node.
   */
  class ProgramAST {
  public:
    /**
     * @brief Construct a new ProgramAST object.
     * @param body The ast nodes.
     */
    explicit ProgramAST(std::vector<ExprPtr> body) noexcept
        : body(std::move(body)) {}

    std::vector<ExprPtr> body; /**< The ast nodes. */
  };

  /**
   * @brief Abstract syntax tree node. This is the base class for all AST nodes.
   */
  class ExprAST {
  public:
    /**
     * @brief The expression type.
     */
    enum class Type : uint8_t {
      NUMBER,   /**< NumberAST. */
      STRING,   /**< StringAST. */
      IDENT,    /**< IdentAST. */
      VAR_DECL, /**< VarDeclAST. */
      BINARY    /**< BinaryAST. */
    };

    /**
     * @brief Construct a new ExprAST object.
     * @param type The expression type.
     */
    explicit ExprAST(Type type) noexcept : type(type) {}

    /**
     * @brief Destroy the ExprAST object.
     */
    virtual ~ExprAST() = default;

    /**
     * @brief Get the string representation of the node.
     * @return The string representation of the node.
     */
    virtual const std::string str() const = 0;

    /**
     * @brief Get the expression type.
     * @return The expression type.
     */
    Type getType() const noexcept { return type; }

  private:
    Type type; /**< The expression type. */
  };

  /**
   * @brief Number AST node.
   * \note
   *  literal: 1, 2, 3, ...
   *  types: NUMBER
   */
  class NumberAST : public ExprAST {
  public:
    /**
     * @brief Construct a new NumberAST object.
     * @param value The number value.
     */
    explicit NumberAST(double value) noexcept
        : ExprAST(Type::NUMBER), value(value) {}

    /**
     * @brief Get the string representation of the node.
     * @return The string representation of the node.
     */
    const std::string str() const override { return std::to_string(value); }

  protected:
    double value; /**< The number value. */
  };

  /**
   * @brief String AST node.
   * \note
   *  literal: "hello", "world", ...
   *  types: STRING
   */
  class StringAST : public ExprAST {
  public:
    /**
     * @brief Construct a new StringAST object.
     * @param value The string value.
     */
    explicit StringAST(std::string value) noexcept
        : ExprAST(Type::STRING), value(std::move(value)) {}

    /**
     * @brief Get the string representation of the node.
     * @return The string representation of the node.
     */
    const std::string str() const override {
      return std::format("\"{}\"", value);
    }

  private:
    std::string value; /**< The string value. */
  };

  /**
   * @brief Identifier AST node.
   * \note
   *  literal: x, y, z, ...
   *  types: IDENT
   */
  class IdentAST : public ExprAST {
  public:
    /**
     * @brief Construct a new IdentAST object.
     * @param name The identifier name.
     */
    explicit IdentAST(std::string name) noexcept
        : ExprAST(Type::IDENT), name(std::move(name)) {}

    /**
     * @brief Get the string representation of the node.
     * @return The string representation of the node.
     */
    const std::string str() const override { return name; }

  private:
    std::string name; /**< The identifier name. */
  };

  /**
   * @brief Variable declaration AST node.
   * \note
   * types: IDENTIFIER COLON IDENTIFIER EQUAL expr
   */
  class VarDeclAST : public ExprAST {
  public:
    /**
     * @brief Construct a new VarDeclAST object.
     * @param name The variable name.
     * @param type The variable type.
     * @param expr The expression.
     */
    VarDeclAST(std::string name, std::string data_type, ExprPtr expr) noexcept
        : ExprAST(Type::VAR_DECL), name(std::move(name)),
          data_type(std::move(data_type)), value(std::move(expr)) {}

    /**
     * @brief Get the string representation of the node.
     * @return The string representation of the node.
     */
    const std::string str() const override {
      return std::format("{}: {} = {}", name, data_type, value->str());
    }

  private:
    std::string data_type; /**< The variable data type. */
    std::string name;      /**< The variable name. */
    ExprPtr value;         /**< The value of the variable.. */
  };

  /**
   * @brief Binary AST node.
   * \note
   *  op: '+', '-', '*', '/', '<', '>', '==', '!=', '<=', '>='
   *  types: PLUS, MINUS, STAR, SLASH, LESS, GREATER, EQUAL_EQUAL, BANG_EQUAL,
   */
  class BinaryAST : public ExprAST {
  public:
    /**
     * @brief Construct a new BinaryAST object.
     * @param op The binary operator.
     * @param lhs The left-hand side expression.
     * @param rhs The right-hand side expression.
     */
    BinaryAST(std::string op, ExprPtr lhs, ExprPtr rhs) noexcept
        : ExprAST(Type::BINARY), op(op), lhs(std::move(lhs)),
          rhs(std::move(rhs)) {}

    /**
     * @brief Get the string representation of the node.
     * @return The string representation of the node.
     */
    const std::string str() const override {
      std::string lhs_str = lhs->str();
      std::string rhs_str = rhs->str();

      return "(" + lhs_str + " " + op + " " + rhs_str + ")";
    }

  private:
    std::string op;               /**< The binary operator. */
    std::unique_ptr<ExprAST> lhs; /**< The left-hand side expression. */
    std::unique_ptr<ExprAST> rhs; /**< The right-hand side expression. */
  };
} // namespace excerpt

#endif // EXCERPT_AST_HPP
