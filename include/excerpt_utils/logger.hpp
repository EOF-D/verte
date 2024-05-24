/**
 * @file logger.hpp
 * @brief Header file for the Logger class.
 *
 * Example usage:
 * \code{.cpp}
 * #include "logger.hpp"
 *
 * int main() {
 *  excerpt::utils::Logger logger("main-demo");
 *
 *  logger.info("some info message.");
 *  logger.warn("some warning message.");
 *  logger.error("some error message.");
 *
 *  logger.info("some info message with arguments: {}", 42);
 *  logger.warn("Hello {}", "world!");
 *
 *  return 0;
 * }
 * \endcode
 */

#ifndef EXCERPT_LOGGER_HPP
#define EXCERPT_LOGGER_HPP

#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace excerpt::utils {
  using LogData = std::pair<std::string_view, std::string_view>;

  /**
   * @brief Concept to check if a type supports the `<<` operator.
   * @tparam T The type to check.
   */
  template <typename T>
  concept OutputStream =
      requires(T &stream, typename T::char_type c) { stream << c; };

  /**
   * @brief The log levels
   */
  enum class LogLevel : uint8_t {
    NONE,    /**< No logging. */
    INFO,    /**< Informational message. */
    WARNING, /**< Warning message. */
    ERROR,   /**< Error message. */
  };

  namespace Logging {
    inline LogLevel globalLevel = LogLevel::NONE; /**< The global level. */

    /**
     * @brief Sets the global log level.
     * @param level The LogLevel to set as global.
     */
    inline void setLevel(LogLevel level) { globalLevel = level; }

    /**
     * @brief Get the global log level.
     * @return The global log level.
     */
    inline LogLevel getLevel() { return globalLevel; }
  } // namespace Logging

  /**
   * @brief Logger class for logging messages with different levels.
   * - Log Levels
   *  -# NONE
   *  -# INFO
   *  -# WARNING
   *  -# ERROR
   */
  class Logger {
  public:
    /**
     * @brief Construct a new Logger object.
     * @param name The name of the logger.
     *
     * \note
     *  This constructor uses the default output stream `std::cout`.
     */
    explicit Logger(const std::string &name) noexcept
        : name(name), output(std::cout) {}

    /**
     * @brief Construct a new Logger object.
     * @tparam Stream The output stream type.
     * @param name The name of the logger.
     * @param stream The output stream.
     *
     * \note
     *  This constructor is only available if the output stream type
     *  supports the `<<` operator.
     */
    template <OutputStream Stream>
    explicit Logger(const std::string &name, Stream &stream) noexcept
        : name(name), output(stream) {}

    /**
     * @brief Log an info message.
     * @tparam Args The types of the variadic arguments.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void info(const std::string &message, Args &&...args) {
      log(LogLevel::INFO, message, std::forward<Args>(args)...);
    }

    /**
     * @brief Log a warning message.
     * @tparam Args The types of the variadic arguments.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void warn(const std::string &message, Args &&...args) {
      log(LogLevel::WARNING, message, std::forward<Args>(args)...);
    }

    /**
     * @brief Log an error message>
     * @tparam Args The types of the variadic arguments.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void error(const std::string &message, Args &&...args) {
      log(LogLevel::ERROR, message, std::forward<Args>(args)...);
    }

  private:
    /**
     * @brief Log a message with a specified level.
     * @tparam Args The types of the variadic arguments.
     * @param level The log level.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void log(LogLevel level, const std::string &message, Args &&...args) {
      // Check if the log level is less than the global level.
      if (Logging::getLevel() < level)
        return;

      // Get the current timestamp & format.
      const auto time = std::chrono::current_zone()->to_local(
          std::chrono::system_clock::now());

      const std::string timestamp =
          std::format("{:%Y-%m-%d %H:%M:%S}",
                      std::chrono::floor<std::chrono::seconds>(time));

      // Unpacking code and prefix.
      const auto &[code, prefix] = LEVEL_DATA[static_cast<uint8_t>(level)];

      // Lock the mutex.
      std::lock_guard<std::mutex> lock(mutex);

      output << "[" + timestamp + "]";
      output << std::format("{}[{}:{}]: \033[0m ", code, name, prefix);
      output << std::vformat(message, std::make_format_args(args...));
      output << "\n";
    }

    static constexpr std::array<LogData, 4> LEVEL_DATA = {{
        {"\033[0m", "NONE"},
        {"\033[0;32m", "INFO"},
        {"\033[0;33m", "WARNING"},
        {"\033[0;31m", "ERROR"},
    }}; /**< The log level data. */

    std::string name;     /**< The name of the logger. */
    std::ostream &output; /**< The output stream. */
    std::mutex mutex;     /**< The mutex to lock the output stream. */
  };
} // namespace excerpt::utils

#endif // EXCERPT_LOGGER_HPP
