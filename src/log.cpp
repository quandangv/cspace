#include "log.hpp"

#include <iostream>

namespace log {
  void info(const string& text) {
    std::cerr << "INFO: " << text << std::endl;
  }
  void error(const string& text) {
    std::cerr << "ERROR: " << text << std::endl;
  }
  void debug(const string& text) {
    std::cerr << "DEBUG: " << text << std::endl;
  }
}
