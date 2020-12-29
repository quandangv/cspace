#pragma once

#include "colorspace.hpp"

namespace cspace {
  bool parse(const char*, double&);
  bool parse(const char*, int&);
  bool parse(const char*, bool&);
}
