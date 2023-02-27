#pragma once

#include <ctime>
#include <iostream>
#include <string>

class Logger {
 public:
  /**
   * @brief Construct a new Logger object
   *
   */
  Logger() = default;

  /**
   * @brief Construct a new Logger object
   *
   * @param name - Prefix for the log message
   */
  Logger(const std::string& name) : name(name){};

  /**
   * @brief Log a message
   *
   * @param message - Message to log
   */
  void log(const std::string& message) {
    auto timestamp = getTimestamp();
    std::cout << timestamp << " [" << name << "] " << message << '\n';
  }

 private:
  std::string name = "Default";

  /**
   * @brief Get the ISO Current Timestamp object
   *
   * @return string - ISO Current Timestamp
   */
  std::string getTimestamp() {
    time_t now;
    time(&now);
    char buf[sizeof "0000-00-00T00:00:00Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return buf;
  }
};