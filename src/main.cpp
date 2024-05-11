#include "logger.hpp"

int main() {
  excerpt::utils::Logger logger("main-demo");

  logger.info("some info message.");
  logger.warn("some warning message.");
  logger.error("some error message.");

  logger.info("some info message with arguments: {}", 42);
  logger.warn("Hello {}", "world!");

  return 0;
}
