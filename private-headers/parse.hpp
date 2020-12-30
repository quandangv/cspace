#pragma once

#include "colorspace.hpp"
#include "tstring.hpp"

namespace cspace {
  bool parse(const tstring&, double&);
  bool parse(const tstring&, int&);
  bool parse(const tstring&, bool&);
}
