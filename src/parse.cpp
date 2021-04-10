#include "parse.hpp"
#include "logger.hpp"
#include "tstring.hpp"

#include <sstream>
#include <iomanip>

GLOBAL_NAMESPACE

using namespace std;

bool parse(const tstring& str, double& result) {
  int len;
  return sscanf(str.begin(), "%lf%n", &result, &len) == 1 && len == (int)str.size();
}

bool parse(const tstring& str, int& result) {
  int len;
  return sscanf(str.begin(), "%d%n", &result, &len) == 1 && len == (int)str.size();
}

bool parse(const tstring& str, bool& result) {
  #define CMP(a) strncasecmp(str.begin(), #a, str.size()) == 0
  if (CMP(on) || CMP(true))
    result = true;
  else if (CMP(off) || CMP(false))
    result = false;
  else return false;
  return true;
  #undef CMP
}

// Returns the component index from the component name and the color space
int parse_component(const tstring& name, colorspace space) {
  using namespace colorspaces;
  #define CASE(word, val) if(strncasecmp(name.begin(), #word, name.size()) == 0) return val;
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
  throw parse_error(spacetos(space) + " colorspace: Unknown component: " + name);
  #undef CASE
}

// Parse the color code and set the value to each component
// Returns the maximum value; ie. 15 for 4-bit colors, 255 for 8-bit colors...
// alpha_first informs whether the color code is in the [A]RGB or RGB[A] format
bool parse_hex(const tstring& value, double* comps, bool& have_alpha) {
  bool error = false;
  auto str = value.begin();

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

  switch(value.size()) {
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

void clamp(double& value, double min, double max) {
  if (value < min)
    value = min;
  else if (value > max)
    value = max;
}


GLOBAL_NAMESPACE_END
