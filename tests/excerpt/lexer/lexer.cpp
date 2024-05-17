/**
 * @file token.cpp
 * @brief Unit tests for the Token class.
 */

#include "excerpt/lexer/lexer.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(LexerTest, TestNextToken) {
  std::string source = "1 + 2 * 3.14 \"hello\" true false";
  Lexer lexer(source);

  // clang-format off
  std::vector<Token> expected = {
      Token("1", Token::Type::NUMBER, {1, 1}),
      Token("+", Token::Type::PLUS, {1, 3}),
      Token("2", Token::Type::NUMBER, {1, 5}),
      Token("*", Token::Type::STAR, {1, 7}),
      Token("3.14", Token::Type::NUMBER, {1, 12}),
      Token("hello", Token::Type::STRING, {1, 20}),
      Token("true", Token::Type::TRUE, {1, 25}),
      Token("false", Token::Type::FALSE, {1, 31}),
      Token("\0", Token::Type::END, {1, 31})
  };
  // clang-format on

  for (const auto &expected_token : expected) {
    Token token = lexer.next();
    EXPECT_EQ(token, expected_token)
        << "Expected: " << expected_token.toString()
        << "\nGot: " << token.toString();
  }
}

TEST(LexerTest, TestEof) {
  std::string source = "foo";
  Lexer lexer(source);

  ASSERT_FALSE(lexer.eof());

  (void)lexer.next(); // Silence unused.
  ASSERT_TRUE(lexer.eof());
}

TEST(LexerTest, TestParseKeywords) {
  std::string source = "if else or true false for while super this none fn";
  Lexer lexer(source);

  for (int i = 0; i < 11; i++) {
    auto token = lexer.next();
    ASSERT_TRUE(isKeyword(token))
        << "Expected `" << token.toString() << "` to be a keyword.\n";
  }
}

TEST(LexerTest, TestSymbols) {
  std::string source = "( ) { } [ ] , . ; / * - + ! = < > == != <= >=";
  Lexer lexer(source);

  // clang-format off
  std::vector<Token::Type> expected = {
      Token::Type::LPAREN, Token::Type::RPAREN,
      Token::Type::LBRACE, Token::Type::RBRACE,
      Token::Type::LBRACKET, Token::Type::RBRACKET,
      Token::Type::COMMA, Token::Type::DOT,
      Token::Type::SEMICOLON, Token::Type::SLASH,
      Token::Type::STAR, Token::Type::MINUS,
      Token::Type::PLUS, Token::Type::BANG,
      Token::Type::EQUAL, Token::Type::LESS,
      Token::Type::GREATER, Token::Type::EQUAL_EQUAL,
      Token::Type::BANG_EQUAL, Token::Type::LESS_EQUAL,
      Token::Type::GREATER_EQUAL, Token::Type::END
  };
  // clang-format on

  for (const auto &expectedType : expected) {
    Token token = lexer.next();
    ASSERT_TRUE(token.is(expectedType));
  }
}

TEST(LexerTest, TestParsing) {
  std::string source = "\"string\" 1 1.2 identifier";
  Lexer lexer(source);

  // clang-format off
  std::vector<Token::Type> expected = {
    Token::Type::STRING,
    Token::Type::NUMBER,
    Token::Type::NUMBER,
    Token::Type::IDENTIFIER
  };
  // clang-format on

  for (const auto &expectedType : expected) {
    Token token = lexer.next();
    ASSERT_TRUE(token.is(expectedType));
  }
}

TEST(LexerTest, TestSkip) {
  std::string source = "(* ... *) actual (*\n ... \n*)";
  Lexer lexer(source);

  ASSERT_TRUE(lexer.next().is(Token::Type::IDENTIFIER));
  ASSERT_TRUE(lexer.next().is(Token::Type::END));
}

TEST(LexerTest, TestEscapeSequences) {
  Lexer lexer("\"\\n\\r\\t\\\\\\\"\"");
  Token token = lexer.next();
  EXPECT_EQ(token.value, "\n\r\t\\\"");
}

TEST(LexerTest, TestMultipleEscapeSequences) {
  Lexer lexer("\"\\n\\r\\t\\\\\\\"\\n\\r\\t\\\\\\\"\"");
  Token token = lexer.next();
  EXPECT_EQ(token.value, "\n\r\t\\\"\n\r\t\\\"");
}

TEST(LexerTest, TestInvalidEscapeSequence) {
  Lexer lexer("\"\\x\"");
  EXPECT_THROW((void)lexer.next(), LexerError);
}

TEST(LexerTest, TestUnterminatedString) {
  Lexer lexer("\"Hello");
  EXPECT_THROW((void)lexer.next(), LexerError);
}
