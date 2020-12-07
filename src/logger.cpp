#include "logger.hpp"

#include <iostream>

namespace logger {
  void info(const string& text) {
    std::cerr << "INFO: " << text << std::endl;
  }
  void error(const string& text) {
    std::cerr << "ERROR: " << text << std::endl;
  }
  void warn(const string& text) {
    std::cerr << "WARN: " << text << std::endl;
  }
  void debug(const string& text) {
    std::cerr << "DEBUG: " << text << std::endl;
  }
}
