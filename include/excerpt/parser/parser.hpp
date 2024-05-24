/**
 * @file parser.hpp
 * @brief Handles parsing some tokens to validate them. Creates an AST.
 */

#ifndef EXCERPT_PARSER_HPP
#define EXCERPT_PARSER_HPP

#include "excerpt/lexer/token.hpp"
#include "excerpt/parser/ast.hpp"
#include "excerpt_utils/logger.hpp"

#include <map>
#include <vector>

namespace excerpt {
  /**
   * @brief Parses tokens into an AST.
   */
  class Parser {
  public:
    /**
     * @brief Construct a new Parser object.
     * @param tokens The tokens to parse.
     */
    explicit Parser(std::vector<Token> &tokens) noexcept
        : tokens(tokens), logger("parser"), index(0) {}

    /**
     * @brief Parse the tokens into an AST.
     */
    [[nodiscard]] std::unique_ptr<ProgramAST> parse();

  private:
    /**
     * @brief Parse a statement.
     * @return The parsed statement.
     */
    [[nodiscard]] NodePtr parseStmt();

    /**
     * @brief Parse a variable declaration statement.
     * @return The parsed variable declaration statement.
     */
    [[nodiscard]] NodePtr parseVarDecl();

    /**
     * @brief Parse a function declaration statement.
     * @return The parsed function declaration statement.
     */
    [[nodiscard]] NodePtr parseFuncDecl();

    /**
     * @brief Parse prototype for a function declaration.
     * @return The parsed prototype.
     */
    [[nodiscard]] ProtoPtr parseProto();

    /**
     * @brief Parse a parameter list for a function declaration.
     * @return The parsed parameter list.
     */
    [[nodiscard]] std::vector<Parameter> parseParams();

    /**
     * @brief Parse a single parameter for a function declaration.
     * @return The parsed parameter.
     */
    [[nodiscard]] Parameter parseParam();

    /**
     * @brief Parse a type identifier.
     * @return The parsed type identifier.
     */
    [[nodiscard]] TypeInfo parseType();

    /**
     * @brief Parse a return statement.
     * @return The parsed return statement.
     */
    [[nodiscard]] NodePtr parseReturn();

    /**
     * @brief Parse an expression statement.
     * @return The parsed expression statement.
     */
    [[nodiscard]] NodePtr parseExprStmt();

    /**
     * @brief Parse a block of statements.
     * @return The parsed block of statements.
     */
    [[nodiscard]] BlockPtr parseBlock();

    /**
     * @brief Parse an expression.
     * @return The parsed expression.
     */
    [[nodiscard]] NodePtr parseExpr();

    /**
     * @brief Parse a binary expression.
     * @param min The minimun precedence.
     * @return The parsed binary expression.
     */
    [[nodiscard]] NodePtr parseBinary(int min);

    /**
     * @brief Parse a unary expression.
     * @return The parsed unary expression.
     */
    [[nodiscard]] NodePtr parseUnary();

    /**
     * @brief Parse a primary expression.
     * @return The parsed primary expression.
     */
    [[nodiscard]] NodePtr parsePrimary();

    /**
     * @brief Parse a function call.
     * @param callee The function to call.
     * @return The parsed function call.
     */
    [[nodiscard]] NodePtr parseCall(NodePtr callee);

    /**
     * @brief Get the current token.
     * @return The current token.
     */
    [[nodiscard]] inline Token current() {
      // If we're at the end of the tokens, return EOF.
      if (index >= tokens.size())
        return tokens.back();

      return tokens[index];
    }

    /**
     * @brief Get the next token.
     * @return The next token or EOF if past the max size.
     */
    [[nodiscard]] inline Token next() {
      index++;

      Token token = current();
      if (token.is(Token::Type::END)) {
        index = tokens.size(); // Make sure index is set to size if EOF.
        return tokens.back();
      }

      return token;
    }

    /**
     * @brief Peek at the next token without advancing the index.
     * @param offset The offset to peek, defaulted to `1`.
     * @return The token peeked at, otherwise EOF if offset is past the size.
     */
    [[nodiscard]] inline Token peek(size_t offset = 1) {
      if (index + offset >= tokens.size())
        return tokens.back();

      return tokens[index + offset];
    }

    /**
     * @brief Match the current token to a specific type.
     * @param type The type to match.
     * @return True if the current token matches the type.
     */
    [[nodiscard]] inline bool match(Token::Type type) {
      if (current().is(type)) {
        index++;
        return true;
      }

      return false;
    }

    /**
     * @brief Match the current token to any of the types in a list.
     * @param types The types to match.
     * @return True if the current token matches any of the types.
     */
    [[nodiscard]] inline bool
    match(const std::initializer_list<Token::Type> &types) {
      if (current().isOneOf(types)) {
        index++;
        return true;
      }

      return false;
    }

    /**
     * @brief Handle a parsing error.
     * @param message The error message.
     */
    [[noreturn]] inline void error(const std::string &message) {
      auto [line, column] = current().meta;
      std::string error = std::format("{}:{}: {}", line, column, message);

      logger.error(error);
      throw ParserError(error, line, column);
    }

    /**
     * @brief Shortcut for creating nodes.
     * @tparam T The type of node to create.
     * @tparam Args The types of arguments to forward to the constructor.
     * @param args The arguments to forward to the constructor.
     * @return The created node.
     */
    template <typename T, typename... Args>
    inline NodePtr create(Args... args) {
      return std::make_unique<T>(std::forward<Args>(args)...);
    }

  private:
    size_t index;              /**< The current index in the token stream. */
    std::vector<Token> tokens; /**< The tokens to parse. */
    utils::Logger logger;      /**< The logger for the parser. */
  };

  /**
   * @brief All the binary operator types.
   */
  static constexpr std::initializer_list<Token::Type> BINARY_OPERATOR_TYPES = {
      Token::Type::PLUS,        Token::Type::MINUS,
      Token::Type::STAR,        Token::Type::SLASH,
      Token::Type::EQUAL_EQUAL, Token::Type::BANG_EQUAL,
      Token::Type::LESS,        Token::Type::GREATER,
      Token::Type::LESS_EQUAL,  Token::Type::GREATER_EQUAL,
      Token::Type::OR,          Token::Type::AND};

  /**
   * @brief All the unary operator types.
   */
  // TODO: Add more unary operators.
  static constexpr std::initializer_list<Token::Type> UNARY_OPERATOR_TYPES = {
      Token::Type::PLUS, Token::Type::MINUS, Token::Type::BANG};

  /**
   * @brief Mapping of precedence for operators.
   */
  static const std::map<Token::Type, int> PRECEDENCE = {
      {Token::Type::OR, 1},
      {Token::Type::EQUAL_EQUAL, 2},
      {Token::Type::BANG_EQUAL, 2},
      {Token::Type::LESS, 3},
      {Token::Type::GREATER, 3},
      {Token::Type::LESS_EQUAL, 3},
      {Token::Type::GREATER_EQUAL, 3},
      {Token::Type::PLUS, 4},
      {Token::Type::MINUS, 4},
      {Token::Type::STAR, 5},
      {Token::Type::SLASH, 5},
      {Token::Type::BANG, 6}};

  /**
   * @brief Get the precedence of a token.
   * @param type The token type.
   * @return The precedence.
   */
  [[nodiscard]] static inline int getPrecedence(Token::Type type) {
    if (PRECEDENCE.contains(type))
      return PRECEDENCE.at(type);

    return -1;
  }
} // namespace excerpt

#endif // EXCERPT_PARSER_HPP
