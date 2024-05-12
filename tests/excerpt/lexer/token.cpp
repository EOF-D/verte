/**
 * @file token.cpp
 * @brief Unit tests for the Token class.
 */

#include "excerpt/lexer/token.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(TokenTest, TestTokenToString) {
  // clang-format off
  Token::Meta meta = {0, 0};
  #define add(name, value) \
    {                                                     \
      Token token(value, Token::Type::name, meta);        \
      if (Token::Type::name == Token::Type::END) {        \
        ASSERT_EQ(token.toString(), "END()");             \
      } else {                                            \
        ASSERT_EQ(token.toString(), #name "(" value ")"); \
      }                                                   \
    }
    TOKENS
  #undef add
  // clang-format on
}

TEST(TokenTest, TestTokenIs) {
  // clang-format off
  Token::Meta meta = {0, 0};

  #define add(name, value)                         \
    {                                              \
      Token token(value, Token::Type::name, meta); \
      ASSERT_TRUE(token.is(Token::Type::name));    \
    }
    TOKENS
  #undef add
  // clang-format on
}

TEST(TokenTest, TestTokenIsOneOf) {
  Token::Meta meta = {0, 0};
  auto types = {Token::Type::IDENTIFIER, Token::Type::NUMBER};

  Token token("foo", Token::Type::IDENTIFIER, meta);
  Token token2("123", Token::Type::NUMBER, meta);

  ASSERT_TRUE(token.isOneOf(types));
  ASSERT_TRUE(token2.isOneOf(types));
}

TEST(TokenTest, TestIsKeyword) {
  Token::Meta meta = {0, 0};

  auto keywords = {
      Token("if", Token::Type::IF, meta),
      Token("else", Token::Type::ELSE, meta),
      Token("or", Token::Type::OR, meta),
      Token("true", Token::Type::TRUE, meta),
      Token("false", Token::Type::FALSE, meta),
      Token("for", Token::Type::FOR, meta),
      Token("while", Token::Type::WHILE, meta),
      Token("super", Token::Type::SUPER, meta),
      Token("this", Token::Type::THIS, meta),
      Token("none", Token::Type::NONE, meta),
      Token("fn", Token::Type::FN, meta),
  };

  for (const auto &token : keywords) {
    ASSERT_TRUE(isKeyword(token));
  }
}

TEST(TokenTest, TestEquality) {
  Token::Meta meta = {0, 0};
  Token token1("foo", Token::Type::IDENTIFIER, meta);
  Token token2("foo", Token::Type::IDENTIFIER, meta);

  ASSERT_EQ(token1, token2);
}

TEST(TokenTest, TestInequality) {
  Token::Meta meta = {0, 0};
  Token token1("foo", Token::Type::IDENTIFIER, meta);
  Token token2("bar", Token::Type::IDENTIFIER, meta);

  ASSERT_NE(token1, token2);
}
