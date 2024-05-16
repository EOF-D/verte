/**
 * @brief Handles lexing of the source code.
 * @file lexer.cpp
 */

#include "excerpt/lexer/lexer.hpp"

namespace excerpt {
  [[nodiscard]] Token Lexer::next() {
    char current_char = skipws();
    if (current_char == '\0')
      return Token("\0", Token::Type::END, {line, column});

    if (std::isdigit(current_char))
      return parseNumber();

    else if (std::isalpha(current_char) || current_char == '_')
      return parseIdentifier();

    else if (current_char == '"')
      return parseString();

    return parseSymbol();
  }

  char Lexer::current() {
    if (eof())
      return '\0';

    return source[index];
  }

  char Lexer::peek(size_t offset) {
    if (index + offset >= source.length())
      return '\0';

    return source[index + offset];
  }

  char Lexer::advance() {
    if (eof())
      return '\0';

    char current = source.at(index);

    index++;
    column++;

    if (current == '\n') {
      line++;
      column = 1;
    }

    return current;
  }

  char Lexer::skipws() {
    while (true) {
      // Skip whitespace characters.
      while (std::isspace(current())) {
        advance();
      }

      // Check for comments.
      if (current() == '(' && peek() == '*') {
        // Skip the opening of the multi-line comment.
        advance();
        advance();

        while (true) {
          if (current() == '\0') {
            error("Unterminated multi-line comment");
          } else if (current() == '*' && peek() == ')') {
            // Skip the closing of the multi-line comment.
            advance();
            advance();
            break;
          } else {
            // Skip other characters in the comment.
            advance();
          }
        }
      } else {
        break;
      }
    }

    return current();
  }

  std::string Lexer::walk(const std::function<bool(char)> &predicate) {
    std::string result;

    while (predicate(current())) {
      result += advance();
    }

    return result;
  }

  [[nodiscard]] Token Lexer::parseString() {
    std::string value;

    // Skip the opening quote.
    char quote = advance();
    while (current() != quote) {
      if (current() == '\0') {
        error("Unterminated string");
      }

      // TODO: Handle escape sequences.
      value += advance();
    }

    // Skip the closing quote.
    advance();

    return Token(value, Token::Type::STRING, {line, column});
  }

  [[nodiscard]] Token Lexer::parseNumber() {
    std::string value = walk([](char c) { return std::isdigit(c); });

    // Check for decimal part.
    if (current() == '.' && std::isdigit(peek())) {
      value += advance();
      value += walk([](char c) { return std::isdigit(c); });
    }

    return Token(value, Token::Type::NUMBER, {line, column});
  }

  [[nodiscard]] Token Lexer::parseIdentifier() {
    std::string value =
        walk([](char c) { return std::isalnum(c) || c == '_'; });

    // Check for keywords.
    if (RESERVED.find(value) != RESERVED.end()) {
      return Token(value, RESERVED.at(value), {line, column});
    }

    return Token(value, Token::Type::IDENTIFIER, {line, column});
  }

  [[nodiscard]] Token Lexer::parseSymbol() {
    std::string value = std::string(1, current());

    // Append the next character if it forms a two-character operator.
    if (peek() == '=') {
      advance();
      value += current();
    }

    if (ATOMIC.find(value) != ATOMIC.end()) {
      advance();
      return Token(value, ATOMIC.at(value), {line, column});
    }

    advance();
    return Token(value, Token::Type::INVALID, {line, column});
  }
} // namespace excerpt
