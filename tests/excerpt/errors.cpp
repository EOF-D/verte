/**
 * @file errors.cpp
 * @brief Tests for the errors.
 */

#include "excerpt/errors.hpp"
#include <gtest/gtest.h>

TEST(InternalErrorTest, TestConstructorAndWhat) {
  const std::string message = "This is an internal error.";
  excerpt::InternalError error(message);

  EXPECT_STREQ(message.c_str(), error.what());
}

TEST(LexerErrorTest, ConstructorAndGetters) {
  const std::string message = "This is a lexer error.";
  const size_t line = 1;
  const size_t column = 2;
  excerpt::LexerError error(message, line, column);

  EXPECT_STREQ(message.c_str(), error.what());
  EXPECT_EQ(line, error.getLine());
  EXPECT_EQ(column, error.getColumn());
}

TEST(ParserErrorTest, ConstructorAndInheritance) {
  const std::string message = "This is a parser error.";
  const size_t line = 1;
  const size_t column = 2;
  excerpt::ParserError error(message, line, column);

  EXPECT_STREQ(message.c_str(), error.what());
  EXPECT_EQ(line, error.getLine());
  EXPECT_EQ(column, error.getColumn());
}
