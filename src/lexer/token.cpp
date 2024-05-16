#include "excerpt/lexer/token.hpp"

namespace excerpt {
  Token::Token(std::string value, Type type, Meta meta) noexcept
      : value(value), type(type), meta(meta) {}

  std::string Token::toString() const {
    // clang-format off
    static const std::string TOKENS_STR[] = {
      #define _(name, value) #name,
        TOKENS
      #undef _
    };

    // clang-format on
    std::string token_type = TOKENS_STR[static_cast<uint8_t>(type)];
    return std::format("({}, {}) {}:{}", value, token_type, meta.line,
                       meta.column);
  }

  bool Token::operator==(const Token &other) const noexcept {
    return type == other.type && value == other.value &&
           (meta.line == other.meta.line && meta.column == other.meta.column);
  }

  bool Token::operator!=(const Token &other) const noexcept {
    return !(*this == other);
  }

  bool Token::is(Type type) const noexcept { return this->type == type; }

  bool Token::isOneOf(std::initializer_list<Type> types) const noexcept {
    for (auto type : types) {
      if (is(type)) {
        return true;
      }
    }

    return false;
  }
} // namespace excerpt
