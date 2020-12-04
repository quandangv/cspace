#pragma once

#include <string>

namespace log {
  using std::string;
  
  void info(const string& text);
  void debug(const string& text);
}
