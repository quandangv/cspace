#include "parse.hpp"
#include "logger.hpp"

#include <sstream>
#include <iomanip>

GLOBAL_NAMESPACE

DEFINE_ERROR(parse_error)

using namespace std;

// Parse the color code and set the value to each component
// Returns the maximum value; ie. 15 for 4-bit colors, 255 for 8-bit colors...
// alpha_first informs whether the color code is in the [A]RGB or RGB[A] format
bool parse_code(const string& value, double* comps, bool& has_alpha) {
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
      has_alpha = true;
      if(false)
    case 3:
        has_alpha = false;
      comps[0] = hex(0) / 15.0;
      comps[1] = hex(1) / 15.0;
      comps[2] = hex(2) / 15.0;
      break;
    case 8:
      comps[3] = ((hex(6) * 16 + hex(7))) / 255.0;
      has_alpha = true;
      if(false)
    case 6:
        has_alpha = false;
      comps[0] = (hex(0) * 16 + hex(1)) / 255.0;
      comps[1] = (hex(2) * 16 + hex(3)) / 255.0;
      comps[2] = (hex(4) * 16 + hex(5)) / 255.0;
      break;
    case 16:
      comps[3] = (hex(12)*4096 + hex(13)*256 + hex(14)*16 + hex(15)) / 65535.0;
      has_alpha = true;
      if(false)
    case 12:
        has_alpha = false;
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

bool parse(const char* str, double& result) {
  int len;
  return sscanf(str, "%lf%n", &result, &len) == 1 && str[len] == 0;
}

bool parse(const char* str, int& result) {
  int len;
  return sscanf(str, "%d%n", &result, &len) == 1 && str[len] == 0;
}

bool parse(const char* str, bool& result) {
  #define CMP(a) strcasecmp(str, #a) == 0
  if (CMP(on) || CMP(true))
    result = true;
  else if (CMP(off) || CMP(false))
    result = false;
  else return false;
  return true;
  #undef CMP
}

GLOBAL_NAMESPACE_END
