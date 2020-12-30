#pragma once

#include "colorspace.hpp"
#include "tstring.hpp"

namespace cspace {
  bool parse(const tstring&, double&);
  bool parse(const tstring&, int&);
  bool parse(const tstring&, bool&);

  int parse_component(const tstring& name, colorspace);
  bool parse_hex(const tstring& value, double* components, bool& have_alpha);
}
