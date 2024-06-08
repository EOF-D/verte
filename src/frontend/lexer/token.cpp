/**
 * @brief Token class implementation.
 * @file tokens.cpp
 */

#include "verte/frontend/lexer/token.hpp"

namespace verte::lexer {
  [[nodiscard]] std::string Token::getValue() const noexcept { return value; }

  std::string Token::toString() const {
    // clang-format off
    static const std::string TOKENS_STR[] = {
      #define _(name, value) #name,
        TOKENS
      #undef _
    };
    // clang-format on

    // Get the token type string and format the token string.
    std::string str = TOKENS_STR[static_cast<uint8_t>(type)];
    return std::format("({}, {}) {}:{}", str, value, meta.line, meta.column);
  }

  bool Token::is(Type type) const noexcept { return this->type == type; }

  bool Token::isOneOf(std::initializer_list<Type> types) const noexcept {
    // Re-use the is() method to check if the token is one of the specified
    for (auto type : types) {
      if (is(type))
        return true;
    }

    return false;
  }

  bool Token::operator==(const Token &other) const noexcept {
    return type == other.type && value == other.value &&
           (meta.line == other.meta.line && meta.column == other.meta.column);
  }

  bool Token::operator!=(const Token &other) const noexcept {
    return !(*this == other);
  }

  inline bool isKeyword(const Token &token) noexcept {
    // clang-format off
    return token.isOneOf({
      Token::Type::IF, Token::Type::ELSE, Token::Type::THEN, 
      Token::Type::OR, Token::Type::AND,
      Token::Type::TRUE, Token::Type::FALSE,
      Token::Type::FOR, Token::Type::WHILE,
      Token::Type::FN, Token::Type::RETURN
    });
    // clang-format on
  }
} // namespace verte::lexer
