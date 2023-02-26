#pragma once

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
    std::cout << "[" << name << "] " << message << '\n';
  }

 private:
  std::string name = "Default";
};