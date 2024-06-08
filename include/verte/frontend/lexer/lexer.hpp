/**
 * @brief Lexer class definition
 * @file lexer.hpp
 */

#ifndef VERTE_FRONTEND_LEXER_LEXER_HPP
#define VERTE_FRONTEND_LEXER_LEXER_HPP

#include "verte/frontend/lexer/token.hpp"
#include "verte/utils/logger.hpp"

#include <functional>
#include <vector>

/**
 * @namespace verte::lexer
 * @brief Lexer namespace, containing lexer-related classes and functions.
 */
namespace verte::lexer {
  /**
   * @class Lexer
   * @brief Handles lexical analysis of the source string.A
   */
  class Lexer {
  public:
    /**
     * @brief Construct a new Lexer object.
     * @param source The source string to lex.
     */
    explicit Lexer(std::string_view source) noexcept
        : source(source), index(0), line(1), column(1), logger("Lexer") {}

    /**
     * @brief Get the next token from the source code.
     * @return The next token from the source code.
     */
    [[nodiscard]] Token nextToken();

    /**
     * @brief Get all tokens from the source code.
     * @return A vector of all tokens from the source code.
     */
    [[nodiscard]] std::vector<Token> allTokens();

    /**
     * @brief Check if the lexer has reached the end of the source code.
     * @return True if the lexer has reached the end of the source code, false
     * otherwise.
     */
    bool atEof() const noexcept;

  private:
    /**
     * @brief Get the current character from the source code.
     * @return The current character from the source code.
     */
    char currentChar() const noexcept;

    /**
     * @brief Get the next character from the source code.
     * @return The next character from the source code.
     */
    char nextChar() noexcept;

    /**
     * @brief Peek at the next character from the source code.
     * @param offset The offset from the current index to peek at. Defaults
     * to 1.
     * @return The next character from the source code.
     */
    char peekChar(size_t offset = 1) const noexcept;

    /**
     * @brief Skip whitespace and skit comments.
     * @return The next non-whitespace/comment character.
     */
    char skip() noexcept;

    /**
     * @brief Skip whitespace characters in the source code.
     * @return The next non-whitespace character.
     */
    char skipWs() noexcept;

    /**
     * @brief Skip comments in the source code.
     * @return The next non-comment character.
     */
    char skipComments() noexcept;

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
    [[noreturn]] void error(const std::string &message);

    const std::string_view source; /**< The source code to tokenize. */

    size_t index;    /**< The current index in the source code. */
    uint32_t line;   /**< The current line number. */
    uint32_t column; /**< The current column number. */

    utils::Logger logger; /**< The logger for the lexer. */
  };
} // namespace verte::lexer

#endif // VERTE_FRONTEND_LEXER_LEXER_HPP
