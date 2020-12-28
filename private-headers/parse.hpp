#pragma once

#include "colorspace.hpp"

namespace cspace {
  bool parse_code(const std::string&, double* comps, bool& has_alpha);

  bool parse(const char*, double&);
  bool parse(const char*, int&);
  bool parse(const char*, bool&);
}
