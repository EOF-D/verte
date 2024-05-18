#ifndef EXCERPT_ERRORS_HPP
#define EXCERPT_ERRORS_HPP

#include <exception>
#include <string>

namespace excerpt {
  /**
   * @breif An exception thrown by the lexer.
   */
  class LexerError : public std::exception {
  public:
    /**
     * @brief Constructs a new LexerError.
     * @param message The error message.
     */
    explicit LexerError(const std::string &message, size_t line, size_t column)
        : message(message), line(line), column(column) {}

    /**
     * @brief Returns the error message.
     * @return The error message.
     */
    const char *what() const noexcept override { return message.c_str(); }

    /**
     * @brief Get the line number.
     * @return The line number.
     */
    size_t getLine() const noexcept { return line; }

    /**
     * @brief Get the column number.
     * @return The column number.
     */
    size_t getColumn() const noexcept { return column; }

  private:
    std::string message; /**< The error message. */
    size_t line;         /**< The line number. */
    size_t column;       /**< The column number. */
  };

  /**
   * @brief An exception thrown by the parser.
   */
  class ParserError : public LexerError {
  public:
    /**
     * @brief Constructs a new ParserError.
     * @param message The error message.
     */
    explicit ParserError(const std::string &message, size_t line, size_t column)
        : LexerError(message, line, column) {}
  };
} // namespace excerpt

#endif // EXCERPT_ERRORS_HPP
