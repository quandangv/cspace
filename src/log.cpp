#include "log.hpp"

#include <iostream>

namespace log {
  void info(const string& text) {
    std::cout << "INFO: " << text << std::endl;
  }
  void debug(const string& text) {
    std::cout << "DEBUG: " << text << std::endl;
  }
}
