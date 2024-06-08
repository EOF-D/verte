/**
 * @brief Lexer class implementation.
 * @file lexer.cpp
 */

#include "verte/frontend/lexer/lexer.hpp"
#include "verte/errors.hpp"

namespace verte::lexer {
  [[nodiscard]] Token Lexer::nextToken() {
    char current_char = skipWs();

    if (current_char == '\0')
      return Token("\0", Token::Type::EOS, {line, column});

    else if (std::isdigit(current_char))
      return parseNumber();

    else if (std::isalpha(current_char) || current_char == '_')
      return parseIdentifier();

    else if (current_char == '"')
      return parseString();

    return parseSymbol();
  }

  [[nodiscard]] std::vector<Token> Lexer::allTokens() {
    std::vector<Token> tokens;

    // Push back tokens until EOS.
    for (auto token = nextToken(); !token.is(Token::Type::EOS);
         token = nextToken()) {
      tokens.emplace_back(std::move(token));
    }

    tokens.push_back(
        Token("END", Token::Type::EOS, {line, column})); // Padding for EOF.

    return tokens;
  }

  bool Lexer::atEof() const noexcept { return index >= source.size(); }

  char Lexer::currentChar() const noexcept {
    return atEof() ? '\0' : source[index];
  }

  char Lexer::nextChar() noexcept {
    if (atEof())
      return '\0';

    // Update line and column.
    if (currentChar() == '\n') {
      line++;
      column = 1;
    }

    else {
      column++;
    }

    return source[index++];
  }

  char Lexer::peekChar(size_t offset) const noexcept {
    if (index + offset >= source.size())
      return '\0';

    return source[index + offset];
  }

  char Lexer::skip() noexcept {
    skipWs(); // Skip whitespace.
    if (currentChar() == '\0')
      return '\0';

    return skipComments(); // Skip comments.
  }

  char Lexer::skipWs() noexcept {
    while (std::isspace(currentChar())) {
      nextChar();
    }

    return currentChar();
  }

  char Lexer::skipComments() noexcept {
    if (currentChar() == '/' && peekChar() == '/') {
      // Skipping `//`.
      nextChar();
      nextChar();

      while (true) {
        char current = currentChar();
        if (current == '\0')
          error("Untermianted comment");

        else if (current == '*' && peekChar() == '/') {
          // Skipping `*/`.
          nextChar();
          nextChar();
          break;
        }

        // Skip other characters in the comment.
        nextChar();
      }
    }

    return currentChar();
  }

  std::string Lexer::walk(const std::function<bool(char)> &predicate) {
    std::string result;

    // Walk through the source code until the predicate is false.
    while (predicate(currentChar())) {
      result += nextChar();
    }

    return result;
  }

  Token Lexer::parseString() {
    std::string value;
    nextChar();

    while ((currentChar() != '\"') && !atEof()) {
      // Handle escape sequences.
      if (currentChar() == '\\') {
        nextChar(); // Skip the backslash.

        // clang-format off
        // Handle escape sequences.
        switch (nextChar()) {
          case 'n': value += '\n'; break;
          case 'r': value += '\r'; break;
          case 't': value += '\t'; break;
          case '\\': value += '\\'; break;
          case '"': value += '"'; break;
          default: error("Invalid escape sequence");
        }
        // clang-format on
      } else {
        value += nextChar();
      }
    }

    if (atEof())
      error("Unterminated string.");

    nextChar(); // Skip the closing quote.
    return Token(value, Token::Type::STRING, {line, column});
  }

  [[nodiscard]] Token Lexer::parseNumber() {
    std::string value = walk([](char c) { return std::isdigit(c); });

    // Check for decimal part.
    if (currentChar() == '.' && std::isdigit(peekChar())) {
      value += nextChar();
      value += walk([](char c) { return std::isdigit(c); });
    }

    return Token(value, Token::Type::NUMBER, {line, column});
  }

  [[nodiscard]] Token Lexer::parseIdentifier() {
    std::string value =
        walk([](char c) { return std::isalnum(c) || c == '_'; });

    // Check for keywords.
    if (tokens::RESERVED.find(value) != tokens::RESERVED.end()) {
      return Token(value, tokens::RESERVED.at(value), {line, column});
    }

    return Token(value, Token::Type::IDENTIFIER, {line, column});
  }

  [[nodiscard]] Token Lexer::parseSymbol() {
    std::string value = std::string(1, currentChar());

    // Append the next character if it forms a two-character operator.
    if (peekChar() == '=') {
      nextChar(); // Skip `=`.
      value += currentChar();
    }

    if (tokens::ATOMIC.find(value) != tokens::ATOMIC.end()) {
      nextChar(); // Go to the next character.
      return Token(value, tokens::ATOMIC.at(value), {line, column});
    }

    nextChar(); // Go to the next character.
    return Token(value, Token::Type::INVALID, {line, column});
  }

  [[noreturn]] void Lexer::error(const std::string &message) {
    std::string error = std::format("{}:{}: {}", line, column, message);
    logger.error(error); // Make sure to log the error. Then throw.

    throw errors::LexicalError(error, line, column);
  }
} // namespace verte::lexer
