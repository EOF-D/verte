/**
 * @brief Errors used by the language.
 * @file errors.hpp
 */

#ifndef VERTE_ERRORS_HPP
#define VERTE_ERRORS_HPP

#include <cstdint>
#include <exception>
#include <source_location>
#include <string>

/**
 * @namespace verte::errors
 * @brief The namespace for errors.
 */
namespace verte::errors {
  /**
   * @typedef Location
   * @brief The type for source locations.
   * @note alias to std::source_location
   */
  using Location = std::source_location;

  /**
   * @class VerteError
   * @brief The base class for all errors.
   */
  class VerteError : public std::exception {
  public:
    /**
     * @brief Constructs a new VerteError.
     * @param message The error message.
     * @param location The location of the error.
     */
    VerteError(const std::string &message,
               const Location &location = Location::current())
        : message(message), location(location) {}

    /**
     * @brief Gets the error message.
     * @return The error message.
     */
    const char *what() const noexcept override { return message.c_str(); }

    /**
     * @brief Gets the location of the error.
     * @return The location of the error.
     */
    const Location &getLocation() const { return location; }

  private:
    std::string message; /**< The error message. */
    Location location;   /**< The location of the error. */
  };

  /**
   * @class LexicalError
   * @brief The error for lexical errors.
   */
  class LexicalError : public VerteError {
  public:
    /**
     * @brief Constructs a new LexicalError.
     * @param message The error message.
     * @param location The location of the error.
     */
    LexicalError(const std::string &message, uint32_t line, uint32_t column)
        : VerteError(message), line(line), column(column) {}

    /**
     * @brief Gets the line of the error from the soruce.
     * @return The line of the error.
     */
    uint32_t getLine() const { return line; }

    /**
     * @brief Gets the column of the error from the source.
     * @return The column of the error.
     */
    uint32_t getColumn() const { return column; }

  private:
    size_t line;   /**< The line of the error. */
    size_t column; /**< The column of the error. */
  };

  /**
   * @class ParserError
   * @brief The error for parser errors.
   */
  class ParserError : public LexicalError {
  public:
    /**
     * @brief Constructs a new ParserError.
     * @param message The error message.
     * @param location The location of the error.
     */
    ParserError(const std::string &message, uint32_t line, uint32_t column)
        : LexicalError(message, line, column) {}
  };
} // namespace verte::errors

#endif // VERTE_ERRORS_HPP
