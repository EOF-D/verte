/**
 * @file tokenizer.hpp
 * @brief Lexer class for tokenizing source.
 */

#ifndef EXCERPT_LEXER_HPP
#define EXCERPT_LEXER_HPP

#include "excerpt/lexer/token.hpp"
#include "excerpt_utils/logger.hpp"

#include <functional>
#include <map>

namespace excerpt {
  /**
   * @brief Command line argument parser
   */
  class Lexer {
  public:
    /**
     * @brief Construct a new Lexer object.
     * @param source The source code to tokenize.
     */
    explicit Lexer(const std::string_view source) noexcept
        : source(source), logger("lexer"), index(0), line(1), column(0) {}

    /**
     * @brief Get the next token from the source.
     * @return The token.
     */
    [[nodiscard]] Token next();

    /**
     * @brief Check if at EOF.
     * @return True if at the end of source, otherwise false.
     */
    inline bool eof() const noexcept { return index >= source.length(); }

  private:
    /**
     * @brief Get's the current character.
     * @return The current character.
     */
    char current();

    /**
     * @brief Peek at the next character without consuming it.
     * @param offset The offset to peek at. Defaults to 1.
     * @return The next character.
     */
    char peek(size_t offset = 1);

    /**
     * @brief Advance the current index and return the next character.
     * @return The next character.
     */
    char advance();

    /**
     * @brief Skips until character is not whitespace or a comment
     * @return The next non-whitespace/comment character
     */
    char skipws();

    /**
     * @brief Walk the tokenizer through until the predicate is false.
     * @param predicate The predicate to match.
     * @return The characters that matched the predicate.
     */
    std::string walk(const std::function<bool(char)> &predicate);

    /**
     * @brief Parses a string literal.
     * @return The string literal or invalid token.
     */
    [[nodiscard]] Token parseString();

    /**
     * @brief Parses a number literal.
     * @return The number literal or invalid token.
     */
    [[nodiscard]] Token parseNumber();

    /**
     * @brief Parses an identifier.
     * @return The identifier or invalid token.
     */
    [[nodiscard]] Token parseIdentifier();

    /**
     * @brief Parses a symbol.
     * @return The symbol or invalid token.
     */
    [[nodiscard]] Token parseSymbol();

    /**
     * @brief Handles an error message.
     * @param message The error message.
     */
    inline void error(const std::string &message) {
      std::string error = std::format("{}:{}: {}", line, column, message);
      logger.error(error);

      // TODO: Make own errors.
      throw std::runtime_error(error);
    }

    const std::string_view source; /**< The source code to tokenize. */

    size_t index;    /**< The current index in the source code. */
    uint32_t line;   /**< The current line number. */
    uint32_t column; /**< The current column number. */

    utils::Logger logger; /**< The logger for the lexer. */
  };

  static const std::map<std::string, Token::Type> RESERVED = {
      {"if", Token::Type::IF},       {"else", Token::Type::ELSE},
      {"or", Token::Type::OR},       {"true", Token::Type::TRUE},
      {"false", Token::Type::FALSE}, {"for", Token::Type::FOR},
      {"while", Token::Type::WHILE}, {"super", Token::Type::SUPER},
      {"this", Token::Type::THIS},   {"none", Token::Type::NONE},
      {"fn", Token::Type::FN}};

  static const std::map<std::string, Token::Type> ATOMIC = {
      {"(", Token::Type::LPAREN},        {")", Token::Type::RPAREN},
      {"{", Token::Type::LBRACE},        {"}", Token::Type::RBRACE},
      {"[", Token::Type::LBRACKET},      {"]", Token::Type::RBRACKET},
      {",", Token::Type::COMMA},         {".", Token::Type::DOT},
      {";", Token::Type::SEMICOLON},     {"/", Token::Type::SLASH},
      {"*", Token::Type::STAR},          {"-", Token::Type::MINUS},
      {"+", Token::Type::PLUS},          {"!", Token::Type::BANG},
      {"=", Token::Type::EQUAL},         {"<", Token::Type::LESS},
      {">", Token::Type::GREATER},       {"==", Token::Type::EQUAL_EQUAL},
      {"!=", Token::Type::BANG_EQUAL},   {"<=", Token::Type::LESS_EQUAL},
      {">=", Token::Type::GREATER_EQUAL}};
} // namespace excerpt

#endif // EXCERPT_LEXER_HPP
