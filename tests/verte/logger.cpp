#include "verte/utils/logger.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

using namespace ::testing;
using namespace verte::utils;

class LoggerTest : public ::testing::Test {
protected:
  void SetUp() override {
    logging::setLevel(LogLevel::ERROR);
    logger = std::make_unique<Logger>("tests", output);
  }

  std::vector<std::string> extract() const {
    std::vector<std::string> logs;
    std::istringstream iss(output.str());

    std::string line;
    while (std::getline(iss, line)) {
      logs.push_back(line);
    }

    return logs;
  }

  std::unique_ptr<Logger> logger;
  std::stringstream output;
};

TEST_F(LoggerTest, TestInfoMessage) {
  logger->info("foo");
  logger->info("bar");
  logger->info("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m baz"));

  output.clear();
}

TEST_F(LoggerTest, TestDebugMessage) {
  logger->debug("foo");
  logger->debug("bar");
  logger->debug("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[38;5;214m[tests:DEBUG]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[38;5;214m[tests:DEBUG]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[38;5;214m[tests:DEBUG]: \x1B[0m baz"));

  output.clear();
}

TEST_F(LoggerTest, TestWarnMessage) {
  logger->warn("foo");
  logger->warn("bar");
  logger->warn("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;33m[tests:WARN]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;33m[tests:WARN]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;33m[tests:WARN]: \x1B[0m baz"));

  output.clear();
}

TEST_F(LoggerTest, TestErrorMessage) {
  logger->error("foo");
  logger->error("bar");
  logger->error("baz");

  const auto logs = extract();
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m foo"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m bar"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m baz"));

  output.clear();
}

TEST_F(LoggerTest, TestVariadicArgs) {
  logger->info("{}, {}, {}", 1, 2, 3);
  logger->info("{}, {}, {}", "foo", "bar", "baz");

  const auto logs = extract();

  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m 1, 2, 3"));
  ASSERT_THAT(logs.at(1),
              EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m foo, bar, baz"));

  output.clear();
}

TEST_F(LoggerTest, TestGlobalLogLevelInfo) {
  logging::setLevel(LogLevel::INFO);

  logger->info("info");
  logger->warn("warn");
  logger->error("error");

  const auto logs = extract();
  ASSERT_EQ(logs.size(), 1);
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m info"));

  output.clear();
}

TEST_F(LoggerTest, TestGlobalLogLevelWarning) {
  logging::setLevel(LogLevel::WARNING);

  logger->info("info");
  logger->warn("warn");
  logger->error("error");

  const auto logs = extract();
  ASSERT_EQ(logs.size(), 2);
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m info"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;33m[tests:WARN]: \x1B[0m warn"));

  output.clear();
}

TEST_F(LoggerTest, TestGlobalLogLevelError) {
  logging::setLevel(LogLevel::ERROR);

  logger->info("info");
  logger->warn("warn");
  logger->error("error");

  const auto logs = extract();
  ASSERT_EQ(logs.size(), 3);
  ASSERT_THAT(logs.at(0), EndsWith("\x1B[0;32m[tests:INFO]: \x1B[0m info"));
  ASSERT_THAT(logs.at(1), EndsWith("\x1B[0;33m[tests:WARN]: \x1B[0m warn"));
  ASSERT_THAT(logs.at(2), EndsWith("\x1B[0;31m[tests:ERROR]: \x1B[0m error"));

  output.clear();
}
