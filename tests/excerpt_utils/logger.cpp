/**
 * @file logger.cpp
 * @brief Unit tests for the Logger class.
 */

#include "logger.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>
#include <sstream>

using namespace excerpt::utils;
using namespace ::testing;

class LoggerTest : public ::testing::Test {
protected:
  void SetUp() override { logger = std::make_unique<Logger>("tests", output); }

  void TearDown() override {
    logger.reset(); // Reset with smart pointer.
  }

  /**
   * @brief Helper method to extract logs.
   * @return The logs as a string.
   */
  std::vector<std::string> extract() {
    std::vector<std::string> logs;
    std::istringstream iss(output.str());

    std::string line;
    while (std::getline(iss, line)) {
      logs.push_back(line);
    }

    return logs;
  }

  std::unique_ptr<Logger> logger; /**< The logger object. */
  std::ostringstream output;      /**< The output stream. */
};

TEST_F(LoggerTest, TestInfoMessage) {
  logger->info("foo");
  logger->info("bar");
  logger->info("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m baz"));

  output.clear(); // Clear the output stream.
}

TEST_F(LoggerTest, TestWarningMessage) {
  logger->warn("foo");
  logger->warn("bar");
  logger->warn("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;33m[tests:WARNING]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;33m[tests:WARNING]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;33m[tests:WARNING]: \x1B[0m baz"));

  output.clear(); // Clear the output stream.
}

TEST_F(LoggerTest, TestErrorMessage) {
  logger->error("foo");
  logger->error("bar");
  logger->error("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m baz"));

  output.clear(); // Clear the output stream.
}

TEST_F(LoggerTest, TestVariadicArgs) {
  logger->info("{}, {}, {}", 1, 2, 3);
  logger->info("{}, {}, {}", "foo", "bar", "baz");

  const auto logs = extract();

  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m 1, 2, 3"));
  ASSERT_THAT(logs.at(1),
              EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m foo, bar, baz"));

  output.clear(); // Clear the output stream.
}
