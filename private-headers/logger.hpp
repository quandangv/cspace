#pragma once
#include "common.hpp"

namespace logger {
  void info(const string& text);
  void error(const string& text);
  void warn(const string& text);
  void debug(const string& text);
}
