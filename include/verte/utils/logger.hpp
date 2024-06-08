/**
 * @brief Handles logging of messages to a stream.
 * @file logger.hpp
 */

#ifndef VERTE_UTILS_LOGGER_HPP
#define VERTE_UTILS_LOGGER_HPP

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/**
 * @namespace verte::utils
 * @brief The namespace for utility functions.
 */
namespace verte::utils {
  /**
   * @brief Type alais for the log data.
   */
  using LogData = std::pair<std::string_view, std::string_view>;

  /**
   * @concept OutputStream
   * @brief Concept for an output stream.
   * @tparam T Type of the stream.
   */
  template <typename T>
  concept OutputStream =
      requires(T &stream, typename T::char_type c) { stream << c; };

  /**
   * @enum LogLevel
   * @brief The different levels of logging.
   */
  enum class LogLevel : uint8_t {
    NONE,    /**< No logging. */
    INFO,    /**< Informational logging. */
    DEBUG,   /**< Debug logging. */
    WARNING, /**< Warning logging. */
    ERROR    /**< Error logging. */
  };

  /**
   * @namespace verte::utils::logging
   * @brief The namespace for global logging configuration.
   */
  namespace logging {
    /**
     * @brief The global log level.
     */
    inline LogLevel globalLevel = LogLevel::NONE;

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
  } // namespace logging

  /**
   * @class Logger
   * @brief Handles logging messages with different levels.
   */
  class Logger {
  public:
    /**
     * @brief Construct a new Logger object.
     * @param name The name of the logger.
     * @note
     *  This constructor uses the default output stream `std::cout`.
     */
    explicit Logger(const std::string &name) noexcept
        : name(name), output(std::cout) {}

    /**
     * @brief Construct a new Logger object.
     * @tparam Stream The output stream type.
     * @param name The name of the logger.
     * @param stream The output stream.
     * @note
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
    void info(const std::string &message, Args &&...args) const {
      log(LogLevel::INFO, message, std::forward<Args>(args)...);
    }

    /**
     * @brief Log a debug message.
     * @tparam Args The types of the variadic arguments.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void debug(const std::string &message, Args &&...args) const {
      log(LogLevel::DEBUG, message, std::forward<Args>(args)...);
    }

    /**
     * @brief Log a warning message.
     * @tparam Args The types of the variadic arguments.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void warn(const std::string &message, Args &&...args) const {
      log(LogLevel::WARNING, message, std::forward<Args>(args)...);
    }

    /**
     * @brief Log an error message>
     * @tparam Args The types of the variadic arguments.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void error(const std::string &message, Args &&...args) const {
      log(LogLevel::ERROR, message, std::forward<Args>(args)...);
    }

  private:
    /**
     * @brief Log a message.
     * @tparam Args The types of the variadic arguments.
     * @param level The LogLevel of the message.
     * @param message The message to log.
     * @param args The variadic arguments to include in the log message.
     */
    template <typename... Args>
    void log(LogLevel level, const std::string &message, Args &&...args) const {
      // Check if the log level is less than the global level.
      if (logging::getLevel() < level)
        return;

      // Get the current timestamp & format.
      const auto time = std::chrono::current_zone()->to_local(
          std::chrono::system_clock::now());

      const std::string timestamp =
          std::format("{:%Y-%m-%d %H:%M:%S}",
                      std::chrono::floor<std::chrono::seconds>(time));

      // Unpacking code and prefix.
      const auto &[code, prefix] = LEVEL_DATA[static_cast<uint8_t>(level)];
      output << "[" + timestamp + "]";
      output << std::format("{}[{}:{}]: \033[0m ", code, name, prefix);
      output << std::vformat(message, std::make_format_args(args...));
      output << "\n";
    }

    /**
     * @brief The data for each log level.
     */
    // clang-format off
    static constexpr std::array<LogData, 5> LEVEL_DATA = {{
        {"\033[0m",        "NONE"},
        {"\033[0;32m",     "INFO"},
        {"\033[38;5;214m", "DEBUG"},
        {"\033[0;33m",     "WARN"},
        {"\033[0;31m",     "ERROR"}
    }};
    // clang-format on

    const std::string name; /**< The name of the logger. */
    std::ostream &output;   /**< The output stream. */
  };
} // namespace verte::utils

#endif // VERTE_UTILS_LOGGER_HPP
