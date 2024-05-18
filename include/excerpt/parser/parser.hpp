/**
 * @brief Parser for source code.
 * @file parser.hpp
 */

#ifndef EXCERPT_PARSER_HPP
#define EXCERPT_PARSER_HPP

#include "../errors.hpp"
#include "excerpt/lexer/token.hpp"
#include "excerpt/parser/ast.hpp"
#include "excerpt_utils/logger.hpp"

#include <initializer_list>
#include <memory>

// TODO: Redo the precedence levels and operators.
// TODO: Implement a better AST printer.
// TODO: Add unit testing.

namespace excerpt {
  static constexpr int PREC_NONE = 0;
  static constexpr int PREC_ASSIGNMENT = 1;
  static constexpr int PREC_CONDITIONAL = 2;
  static constexpr int PREC_LOGICAL_OR = 3;
  static constexpr int PREC_LOGICAL_AND = 4;
  static constexpr int PREC_EQUALITY = 5;
  static constexpr int PREC_COMPARISON = 6;
  static constexpr int PREC_TERM = 7;
  static constexpr int PREC_FACTOR = 8;
  static constexpr int PREC_UNARY = 9;
  static constexpr int PREC_CALL = 10;

  static constexpr std::initializer_list<Token::Type> OPERATOR_TYPES = {
      Token::Type::PLUS,        Token::Type::MINUS,
      Token::Type::STAR,        Token::Type::SLASH,
      Token::Type::EQUAL_EQUAL, Token::Type::BANG_EQUAL,
      Token::Type::LESS,        Token::Type::GREATER,
      Token::Type::LESS_EQUAL,  Token::Type::GREATER_EQUAL,
      Token::Type::OR,          Token::Type::BANG};

  /**
   * @brief Source code parser.
   */
  class Parser {
  public:
    /**
     * @brief Construct a new Parser object.
     * @param tokens The tokens to parse.
     */
    explicit Parser(const std::vector<Token> &tokens) noexcept
        : tokens(tokens), current(tokens.front()), logger("parser"), index(0) {}

    /**
     * @brief Parse a program.
     * @return The root node of the AST.
     */
    [[nodiscard]] std::unique_ptr<ProgramAST> parse();

  private:
    /**
     * @brief Parse a program (list of statements).
     * @return a vector of AST nodes.
     */
    [[nodiscard]] std::vector<ExprPtr> parseProgram();

    /**
     * @brief Parse an expression.
     * @return The AST node.
     */
    [[nodiscard]] ExprPtr parseExpr();

    /**
     * @brief Parse a variable declaration.
     * @return The AST node.
     */
    [[nodiscard]] ExprPtr parseVarDecl();

    /**
     * @brief Parse a binary expression.
     * @param lhs The left-hand side of the expression.
     * @param min The minimum precedence.
     * @return The AST node.
     */
    [[nodiscard]] ExprPtr parseBinaryExpr(ExprPtr lhs, int min);

    /**
     * @brief Parse atoms.
     * @return The AST node.
     */
    [[nodiscard]] ExprPtr parseAtom();

    /**
     * @brief Get the next token.
     * @return The next token.
     */
    [[nodiscard]] inline Token next() {
      index++;

      // If we're at the end of the tokens, return EOF.
      if (index >= tokens.size())
        return tokens.back();

      current = tokens[index];
      return current;
    }

    /**
     * @brief Peek at the next token.
     * @param offset The offset to peek at. Defaults to `1`.
     * @return The next token.
     */
    [[nodiscard]] inline Token peek(size_t offset = 1) {
      if (index + offset >= tokens.size())
        return tokens.back();

      return tokens[index + offset];
    }

    /**
     * @brief Rewind the parser.
     * @param offset The offset to rewind by. Defaults to `1`.
     * @return The previous token.
     */
    [[nodiscard]] inline Token rewind(size_t offset = 1) {
      index -= offset;

      // Ensure we don't go above the token count.
      if (index >= tokens.size()) {
        index = tokens.size() - 1;
        return tokens.back();
      }

      // Ensure we don't go below zero.
      if (index < 0) {
        index = 0;
        return tokens.front();
      }

      current = tokens[index];
      return current;
    }

    /**
     * @brief Advance the source without returning.
     * @param offset The offset to advance by. Defaults to `1`.
     */
    inline void advance(size_t offset = 1) {
      if (index + offset > tokens.size()) {
        index = tokens.size();
        return;
      }

      index += offset;
      current = tokens[index];
    }

    /**
     * @brief Helper method to validate before creating a node.
     * @tparam T The type of the node.
     * @tparam Args The arguments to pass to the node
     * @param expected The expected token type.
     * @return The node, otherwise an error will occur.
     */
    template <typename T, typename... Args>
    inline ExprPtr create(Args... args) {
      return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Expect a token of a specific type.
     * @param type The token type.
     * @param message The error message.
     */
    inline void expect(Token::Type type, const std::string &message) {
      if (!current.is(type))
        error(message);

      // Move to the next token.
      (void)next();
    }

    /**
     * @brief Handle a parsing error.
     * @param message The error message.
     */
    [[noreturn]] inline void error(const std::string &message) {
      auto [line, column] = current.meta;
      std::string error = std::format("{}:{}: {}", line, column, message);

      logger.error(error);
      throw ParserError(error, line, column);
    }

    /**
     * @brief Get the precedence of a token.
     * @param type The token type.
     * @return The precedence.
     */
    [[nodiscard]] static inline int getPrecedence(Token::Type type) {
      // TODO: Implement the rest of the precedence levels/operators.

      switch (type) {
        case Token::Type::EQUAL:
          return PREC_ASSIGNMENT;
        // case Token::Type::QUESTION_MARK:
        //  return PREC_CONDITIONAL;
        case Token::Type::OR:
          return PREC_LOGICAL_OR;
        // case Token::Type::AND:
        // return PREC_LOGICAL_AND;
        case Token::Type::EQUAL_EQUAL:
        case Token::Type::BANG_EQUAL:
          return PREC_EQUALITY;
        case Token::Type::LESS:
        case Token::Type::GREATER:
        case Token::Type::LESS_EQUAL:
        case Token::Type::GREATER_EQUAL:
          return PREC_COMPARISON;
        case Token::Type::PLUS:
        case Token::Type::MINUS:
          return PREC_TERM;
        case Token::Type::STAR:
        case Token::Type::SLASH:
          return PREC_FACTOR;
        // case Token::Type::BANG:
        // case Token::Type::MINUS_MINUS:
        // case Token::Type::PLUS_PLUS:
        //   return PREC_UNARY;
        default:
          return PREC_NONE;
      }
    }

  private:
    std::vector<Token> tokens; /**< The tokens to parse. */
    Token current;             /**< The current token. */
    size_t index;              /**< The current token index. */

    utils::Logger logger; /**< The logger. */
  };
} // namespace excerpt

#endif // EXCERPT_PARSER_HPP
