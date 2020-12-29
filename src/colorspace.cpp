#include "colorspace.hpp"
#include "logger.hpp"

#include "conversion.hpp"

GLOBAL_NAMESPACE

DEFINE_ERROR(colorspace_error)
constexpr const char scope[] = "indirect-convert";

using namespace std;
using namespace colorspaces;

// Parse the color code and set the value to each component
// Returns the maximum value; ie. 15 for 4-bit colors, 255 for 8-bit colors...
// alpha_first informs whether the color code is in the [A]RGB or RGB[A] format
bool parse_hex(const string& value, double* comps, bool& have_alpha) {
  bool error = false;
  auto str = value.c_str();

  // This parses the base-16 digit at the given index
  auto hex = [&] (int index)->unsigned char {
    auto& c = str[index];
    if (c <= '9' && c >= '0')
      return c - '0';
    if (c <= 'F' && c >= 'A')
      return c - 'A' + 10;
    if (c <= 'f' && c >= 'a')
      return c - 'a' + 10;
    error = true;
    return false;
  };

  switch(value.length()) {
    // Use the length of the string to know the bit size of each component and whether there is an alpha component
    case 4:
      comps[3] = hex(3) / 15.0;
      have_alpha = true;
      if(false)
    case 3:
        have_alpha = false;
      comps[0] = hex(0) / 15.0;
      comps[1] = hex(1) / 15.0;
      comps[2] = hex(2) / 15.0;
      break;
    case 8:
      comps[3] = ((hex(6) * 16 + hex(7))) / 255.0;
      have_alpha = true;
      if(false)
    case 6:
        have_alpha = false;
      comps[0] = (hex(0) * 16 + hex(1)) / 255.0;
      comps[1] = (hex(2) * 16 + hex(3)) / 255.0;
      comps[2] = (hex(4) * 16 + hex(5)) / 255.0;
      break;
    case 16:
      comps[3] = (hex(12)*4096 + hex(13)*256 + hex(14)*16 + hex(15)) / 65535.0;
      have_alpha = true;
      if(false)
    case 12:
        have_alpha = false;
      comps[0] = (hex(0)*4096 + hex(1)*256 + hex(2)*16 + hex(3)) / 65535.0;
      comps[1] = (hex(4)*4096 + hex(5)*256 + hex(6)*16 + hex(7)) / 65535.0;
      comps[2] = (hex(8)*4096 + hex(9)*256 + hex(10)*16 + hex(11)) / 65535.0;
      break;
    default:
      return false;
  }
  if(error)
    return false;
  return true;
}

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
  #define CASE(c) if (strcasecmp(value.c_str(), #c) == 0) return c;
  APPLY_COLORSPACES
  #undef CASE
  throw colorspace_error("Unknown colorspace: "s + value);
}

// Converts colorspace to string
string to_string(colorspace value) {
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
  throw colorspace_error("Color-Mask: Unknown color space: " + to_string(c));
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
    default: throw colorspace_error("Color-Direct: Unknown color conversion: from " + to_string(from) + " to " + to_string(to)); 
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
  logger::debug<scope>("Convert from " + to_string(from) + " to " + to_string(to));
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

// Returns the component index from the component name and the color space
int parse_component(const char* name, colorspace space) {
  #define CASE(word, val) if(strcasecmp(name, #word) == 0) return val;
  if (space == rgb) {
    CASE(red, 0)
    CASE(green, 1)
    CASE(blue, 2)
  }
  if (space == cmyk) {
    CASE(cyan, 0)
    CASE(magenta, 1)
    CASE(yellow, 2)
    CASE(key, 3)
    CASE(black, 3)
  }
  if (space == xyz) {
    CASE(x, 0)
    CASE(y, 1)
    CASE(z, 2)
  }
  if (space == cielab || space == jzazbz || space == cielch || space == jzczhz) {
    CASE(brightness, 0)
    CASE(lightness, 0)
    CASE(lum, 0)
    CASE(luminosity, 0)
  }
  if (space == cielab || space == jzazbz) {
    CASE(a, 1)
    CASE(b, 2)
  }
  if (space == cielch || space == jzczhz) {
    CASE(c, 1)
    CASE(chroma, 1)
    CASE(h, 2)
    CASE(hue, 2)
  }
  if (space == cielab || space == cielch) {
    CASE(l, 0)
  }
  if (space == jzazbz || space == jzczhz) {
    CASE(j, 0)
  }
  if (space == hsl || space == hsv) {
    CASE(h, 0)
    CASE(hue, 0)
    CASE(s, 0)
    CASE(sat, 0)
    CASE(saturation, 0)
  }
  if (space == hsl) {
    CASE(lightness, 2)
    CASE(lum, 2)
    CASE(luminosity, 2)
    CASE(l, 2)
  }
  if (space == hsv) {
    CASE(value, 2)
    CASE(v, 2)
  }
  throw colorspace_error(to_string(space) + " colorspace: Unknown component: " + name);
  #undef CASE
}

GLOBAL_NAMESPACE_END
