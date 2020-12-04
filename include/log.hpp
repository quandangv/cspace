#pragma once

#include <string>

#include "option.hpp"

namespace log {
  using std::string;
  
  void info(const string& text);
  void debug(const string& text);

  template<const char* scope>
  void debug(const string& text) {
    if constexpr(DEBUG_SCOPES.find(scope) != std::string_view::npos)
      debug(text);
  }
}
