#include "colorspace.hpp"
#include "logger.hpp"
#include "conversion.hpp"

#include <cstring>

GLOBAL_NAMESPACE

using namespace std;
using namespace colorspaces;

// All colorspaces goes here
#define APPLY_COLORSPACES \
  CASE(xyz); \
  CASE(rgb); \
  CASE(cmyk); \
  CASE(jzazbz); \
  CASE(cielab); \
  CASE(hsl); \
  CASE(hsv); \
  CASE(jzczhz); \
  CASE(cielch);

// Converts string to colorspace
colorspace stospace(const string& value) {
  #define CASE(c) if (strncasecmp(value.c_str(), #c, value.size()) == 0) return c;
  APPLY_COLORSPACES
  if (strncasecmp(value.c_str(), "lab", value.size()) == 0) return cielab;
  if (strncasecmp(value.c_str(), "lch", value.size()) == 0) return cielch;
  #undef CASE
  throw colorspace_error("Unknown colorspace: "s + value);
}

// Converts colorspace to string
string spacetos(colorspace value) {
  #define CASE(c) if (value == c) return #c;
  APPLY_COLORSPACES
  #undef CASE
  throw colorspace_error("Unknown colorspace: "s + to_string(value));
}

// Returns a string containing all supported colorspaces
string list_colorspaces(const string& sep) {
  string result;
  bool first = true;
  #define CASE(c) if (first) { result += #c; first = false; } else { result += sep + #c; }
  APPLY_COLORSPACES
  #undef CASE
  return result;
}
#undef APPLY_COLORSPACES

int component_count(colorspace c) {
  if (c == cmyk)
    return 4;
  return 3;
}

// All the colorspace branches that have children goes here
colorspace get_mask(colorspace c) {
  #define ENTRY(color) if (c == color) return color##_msk;
  ENTRY(xyz)
  ENTRY(rgb)
  ENTRY(jzazbz)
  ENTRY(cielab)
  #undef ENTRY
  throw colorspace_error("Color-Mask: Unknown color space: " + spacetos(c));
}

// All the direct conversions goes here
void direct(double* value, colorspace from, colorspace to) {
  switch (from << 8 | to) {
  #define CASE_BASE(a, b, method) case a << 8 | b: method(value, value); break;
  #define CASE(a, b) CASE_BASE(a, b, a##_##b) CASE_BASE(b, a, b##_##a)
  #define AB_CH(_ab, _ch) CASE_BASE(_ab, _ch, ab_ch) CASE_BASE(_ch, _ab, ch_ab)
    CASE(rgb, xyz)
    CASE(rgb, hsl)
    CASE(rgb, hsv)
    CASE(rgb, cmyk)
    CASE(xyz, jzazbz)
    CASE(xyz, cielab)
    AB_CH(jzazbz, jzczhz)
    AB_CH(cielab, cielch)
    default: throw colorspace_error("Color-Direct: Unknown color conversion: from " + spacetos(from) + " to " + spacetos(to)); 
  #undef AB_CH
  #undef CASE
  #undef CASE_BASE
  }
}

// Convert from a colorspace to its ancestor
void convert_up(double* value, colorspace from, colorspace parent) {
  if (from == parent) return;
  auto next_mask = from & get_mask(parent);
  convert_up(value, from, next_mask);
  direct(value, next_mask, parent);
}

// Convert from a colorspace to its descendant
void convert_down(double* value, colorspace parent, colorspace to) {
  if (parent == to) return;
  auto next_mask = to & get_mask(parent);
  direct(value, parent, next_mask);
  convert_down(value, next_mask, to);
}

// Indirect conversion happens here
void convert(double* value, colorspace from, colorspace to) {
  if (from == to) return;
  auto parent = 0;

  // Find last common ancestor
  colorspace common;
  while ((common = from & get_mask(parent)) == (to & get_mask(parent))) {
    parent = common;
  }
  convert_up(value, from, parent);
  convert_down(value, parent, to);
}

void clamp(double& value, double min, double max) {
  if (value < min)
    value = min;
  else if (value > max)
    value = max;
}

// Clamp color data to the right range of the specified color space
void clamp(double* value, colorspace space) {
  switch (space) {
  case cmyk:
    clamp(value[3], 0, 1);
    [[fallthrough]];
  case rgb:
    clamp(value[0], 0, 1);
    clamp(value[1], 0, 1);
    clamp(value[2], 0, 1);
    break;
  case hsl:
  case hsv:
    clamp(value[0], 0, 360);
    clamp(value[1], 0, 1);
    clamp(value[2], 0, 1);
    break;
  case cielab:
    clamp(value[0], 0, 1);
    break;
  }
}

GLOBAL_NAMESPACE_END
