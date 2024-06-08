/**
 * @brief Token class definition.
 * @file token.hpp
 */

#ifndef VERTE_FRONTEND_LEXER_TOKENS_H
#define VERTE_FRONTEND_LEXER_TOKENS_H

#include "verte/frontend/lexer/defs.h"

#include <cstdint>
#include <format>
#include <unordered_map>

/**
 * @namespace verte::lexer
 * @brief Lexer namespace, containing lexer-related classes and functions.
 */
namespace verte::lexer {
  /**
   * @class Token
   * @brief Token structure used by the lexer.
   */
  class Token {
  public:
    /**
     * @enum Type
     * @brief Enumeration representing the different types of tokens.
     *
     * This enum defines the various token types used in the lexer, including:
     *   - Symbols (parentheses, braces, brackets, punctuation marks)
     *   - Operators (assignment, arithmetic, logical, relational)
     *   - Keywords (control flow, function definitions, bools, literals)
     *   - Types (identifiers, string literals, number literals)
     *   - Special tokens (invalid, end-of-stream)
     *
     * The token types are defined using macros in the
     * `verte/frontend/lexer/defs.h` file.
     */
    // clang-format off
    enum class Type : uint8_t {
      #define _(name, str) name,
        TOKENS /**< @brief Token types. See above note. */
      #undef _
    } type{Type::INVALID};
    // clang-format on

    /**
     * @struct Meta
     * @brief Token meta information.
     */
    struct Meta {
      uint32_t line;   /**< The line number. */
      uint32_t column; /**< The column number. */
    } meta;

    /**
     * @brief Construct a new Token object.
     * @param value The token value.
     * @param line The line number.
     * @param column The column number.
     */
    Token(std::string value, Type type, Meta meta) noexcept
        : value(value), type(type), meta(meta) {}

    /**
     * @brief Get the token value.
     * @return The token value.
     */
    [[nodiscard]] std::string getValue() const noexcept;

    /**
     * @brief Token to string
     * @return Token string representation
     */
    std::string toString() const;

    /**
     * @brief Check if the token is of a specific type.
     * @param type The token type.
     * @return true if the token is of the specified type, false otherwise.
     */
    bool is(Type type) const noexcept;

    /**
     * @brief Check if the token is one of the specified types.
     * @param types The token types.
     * @return true if the token is one of the specified types, false
     * otherwise.
     */
    bool isOneOf(std::initializer_list<Type> types) const noexcept;

    /**
     * @brief Overload the == operator for Token objects
     * @param other The other Token object
     * @return true if the tokens are equal, false otherwise
     */
    bool operator==(const Token &other) const noexcept;

    /**
     * @brief Overload the != operator for Token objects
     * @param other The other Token object
     * @return true if the tokens are not equal, false otherwise
     */
    bool operator!=(const Token &other) const noexcept;

  private:
    std::string value; /**< Token value. */
  };

  /**
   * @brief Check if a token is a keyword.
   * @param token The token to check.
   * @return true if the token is a keyword, false otherwise.
   */
  inline bool isKeyword(const Token &token) noexcept;
} // namespace verte::lexer

/**
 * @namespace verte::tokens
 * @brief Token map definitions.
 */
namespace verte::tokens {
  using namespace verte::lexer;

  /**
   * @brief Reserved keywords.
   */
  // clang-format off
  static const std::unordered_map<std::string, lexer::Token::Type> RESERVED = {
    #define _(name, value) {value, lexer::Token::Type::name},
      TOKENS
    #undef _
  };

  /**
   * @brief Atomic symbols and operators.
   */
  static const std::unordered_map<std::string, lexer::Token::Type> ATOMIC = {
    #define _(name, value) {value, lexer::Token::Type::name},
      SYMBOLS
      OPERATORS
    #undef _
  };
  // clang-format on

  /**
   * @brief All the binary operator types.
   */
  inline static constexpr std::initializer_list<Token::Type>
      BINARY_OPERATOR_TYPES = {
          Token::Type::PLUS,     Token::Type::MINUS,   Token::Type::STAR,
          Token::Type::SLASH,    Token::Type::EQUAL,   Token::Type::NEQ_EQUAL,
          Token::Type::LESS,     Token::Type::GREATER, Token::Type::LT_EQUAL,
          Token::Type::GT_EQUAL, Token::Type::OR,      Token::Type::AND};

  /**
   * @brief All the unary operator types.
   */
  // TODO: Add more unary operators.
  inline static constexpr std::initializer_list<Token::Type>
      UNARY_OPERATOR_TYPES = {Token::Type::PLUS, Token::Type::MINUS,
                              Token::Type::BANG};

  /**
   * @brief Mapping of precedence for operators.
   */
  inline static const std::unordered_map<Token::Type, int> PRECEDENCE = {
      {Token::Type::OR, 1},        {Token::Type::EQUAL, 2},
      {Token::Type::NEQ_EQUAL, 2}, {Token::Type::LESS, 3},
      {Token::Type::GREATER, 3},   {Token::Type::LT_EQUAL, 3},
      {Token::Type::GT_EQUAL, 3},  {Token::Type::PLUS, 4},
      {Token::Type::MINUS, 4},     {Token::Type::STAR, 5},
      {Token::Type::SLASH, 5},     {Token::Type::BANG, 6}};
} // namespace verte::tokens

#endif // VERTE_FRONTEND_LEXER_TOKENS_H
