#pragma once

#include "colorspace.hpp"
#include "tstring.hpp"
#include "error.hpp"

namespace cspace {
  struct parse_error : error_base { using error_base::error_base; };

  bool parse(const tstring&, double&);
  bool parse(const tstring&, int&);
  bool parse(const tstring&, bool&);

  int parse_component(const tstring& name, colorspace);
  bool parse_hex(const tstring& value, double* components, bool& have_alpha);
  void clamp(double& value, double min, double max);
}
