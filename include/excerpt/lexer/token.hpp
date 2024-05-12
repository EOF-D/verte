/**
 * @file token.hpp
 * @brief Token class, used in the tokenizer.
 */

#ifndef EXCERPT_TOKEN_HPP
#define EXCERPT_TOKEN_HPP

#include "llvm/ADT/StringRef.h"

// clang-format off
namespace excerpt {
  #define TOKENS                  \
    add(LPAREN, "(")              \
    add(RPAREN, ")")              \
    add(LBRACE, "{")              \
    add(RBRACE, "}")              \
    add(COMMA, ",")               \
    add(DOT, ".")                 \
    add(MINUS, "-")               \
    add(PLUS, "+")                \
    add(SEMICOLON, ";")           \
    add(SLASH, "/")               \
    add(STAR, "*")                \
    add(BANG, "!")                \
    add(BANG_EQUAL, "!=")         \
    add(EQUAL, "=")               \
    add(EQUAL_EQUAL, "==")        \
    add(GREATER, ">")             \
    add(GREATER_EQUAL, ">=")      \
    add(LESS, "<")                \
    add(LESS_EQUAL, "<=")         \
    add(IDENTIFIER, "IDENTIFIER") \
    add(STRING, "STRING")         \
    add(NUMBER, "NUMBER")         \
    add(IF, "if")                 \
    add(ELSE, "else")             \
    add(OR, "or")                 \
    add(TRUE, "true")             \
    add(FALSE, "false")           \
    add(FOR, "for")               \
    add(WHILE, "while")           \
    add(SUPER, "super")           \
    add(THIS, "this")             \
    add(NONE, "none")             \
    add(FN, "fn")                 \
    add(END, "\0")                \
    add(INVALID, "INVALID")
  // clang-format on

  using Iterator = llvm::StringRef::iterator;

  /**
   * @brief Command line argument parser
   */
  struct Token {
    llvm::StringRef value; /**< The token value. */

    /**
     * @brief Token types.
     */
    // clang-format off
    enum class Type : uint8_t {
      #define add(name, _) name,
        TOKENS
      #undef add
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
    explicit Token(llvm::StringRef value, Type type, Meta meta) noexcept
        : value(value), type(type), meta(meta) {}

    /**
     * @brief Token to string
     * @return Token string representation
     */
    std::string toString() const {
      // clang-format off
      static const std::string TOKENS_STR[] = {
        #define add(name, value) #name,
          TOKENS
        #undef add
      };

      // clang-format on
      return TOKENS_STR[static_cast<uint8_t>(type)] + "(" + value.str() + ")";
    }

    /**
     * @brief Check if the token is of a specific type.
     * @param type The token type.
     * @return true if the token is of the specified type, false otherwise.
     */
    bool is(Type type) const noexcept { return this->type == type; }

    /**
     * @brief Check if the token is one of the specified types.
     * @param types The token types.
     * @return true if the token is one of the specified types, false
     * otherwise.
     */
    bool isOneOf(std::initializer_list<Type> types) const noexcept {
      for (auto type : types) {
        if (is(type)) {
          return true;
        }
      }

      return false;
    }

    /**
     * @brief Overload the == operator for Token objects
     * @param other The other Token object
     * @return true if the tokens are equal, false otherwise
     */
    bool operator==(const Token &other) const noexcept {
      return type == other.type && value == other.value;
    }

    /**
     * @brief Overload the != operator for Token objects
     * @param other The other Token object
     * @return true if the tokens are not equal, false otherwise
     */
    bool operator!=(const Token &other) const noexcept {
      return !(*this == other);
    }
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
