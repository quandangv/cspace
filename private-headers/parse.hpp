#pragma once

#include "colorspace.hpp"

namespace cspace {
  std::string to_string(const double* value, colorspace);
  std::string to_string(const double* value, int count);

  using component = unsigned int;
  int parse_code(const std::string&, component* comps, bool& has_alpha);

  bool parse(const char*, double&);
  bool parse(const char*, int&);
  bool parse(const char*, bool&);
}
