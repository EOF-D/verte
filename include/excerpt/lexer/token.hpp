/**
 * @file token.hpp
 * @brief Token class, used in the tokenizer.
 */

#ifndef EXCERPT_TOKEN_HPP
#define EXCERPT_TOKEN_HPP

#include <cstdint>
#include <format>
#include <string>

#include "defs.h"

// clang-format off
namespace excerpt {
  /**
   * @brief Command line argument parser
   */
  struct Token {
    std::string value; /**< The token value. */

    /**
     * @brief Token types.
     */
    // clang-format off
    enum class Type : uint8_t {
      #define _(name, _) name,
        TOKENS
      #undef _
      // clang-format on
    } type{Type::INVALID};

    /**
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
    explicit Token(std::string value, Type type, Meta meta) noexcept;

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
  };

  /**
   * @brief Check if a token is a keyword.
   * @param token The token to check.
   * @return true if the token is a keyword, false otherwise.
   */
  inline bool isKeyword(const Token &token) noexcept {
    // clang-format off
    return token.isOneOf({
      Token::Type::IF, Token::Type::ELSE, Token::Type::OR,
      Token::Type::TRUE, Token::Type::FALSE,
      Token::Type::FOR, Token::Type::WHILE,
      Token::Type::SUPER, Token::Type::THIS,
      Token::Type::NONE, Token::Type::FN
    });
  }

}; // namespace excerpt

#endif // EXCERPT_TOKEN_HPP
