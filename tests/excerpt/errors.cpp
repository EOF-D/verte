/**
 * @file errors.cpp
 * @brief Tests for the errors.
 */

#include "excerpt/errors.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(InternalErrorTest, TestConstructorAndWhat) {
  const std::string message = "This is an internal error.";
  InternalError error(message);

  EXPECT_STREQ(message.c_str(), error.what());
}

TEST(LexerErrorTest, TestConstructorAndGetters) {
  const std::string message = "This is a lexer error.";
  const size_t line = 1;
  const size_t column = 2;
  LexerError error(message, line, column);

  EXPECT_STREQ(message.c_str(), error.what());
  EXPECT_EQ(line, error.getLine());
  EXPECT_EQ(column, error.getColumn());
}

TEST(ParserErrorTest, TestConstructorAndInheritance) {
  const std::string message = "This is a parser error.";
  const size_t line = 1;
  const size_t column = 2;
  ParserError error(message, line, column);

  EXPECT_STREQ(message.c_str(), error.what());
  EXPECT_EQ(line, error.getLine());
  EXPECT_EQ(column, error.getColumn());
}
