#include "parse.hpp"

#include <sstream>
#include <iomanip>

#include "logger.hpp"

GLOBAL_NAMESPACE

using namespace std;

// Convert the right number components of the given colorspace to string
string to_string(const double* arr, colorspace space) {
  return to_string(arr, colorspaces::component_count(space));
}

string to_string(const double* arr, int count) {
  if (count == 0) return "";
  stringstream result;
  result  << arr[0];
  for(int i = 1; i < count; i++)
    result << ' ' << arr[i];
  return result.str();
}

// Parse the color code and set the value to each component
// Returns the maximum value; ie. 15 for 4-bit colors, 255 for 8-bit colors...
// alpha_first informs whether the color code is in the [A]RGB or RGB[A] format
int parse_code(const string& value, component* comps, bool& has_alpha) {
  bool error = false;
  int divider;
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
    return 0;
  };

  switch(value.length()) {
    // Use the length of the string to know the bit size of each component and whether there is an alpha component
    case 4:
      comps[3] = hex(3);
      has_alpha = true;
      if(false)
    case 3:
        has_alpha = false;
      comps[0] = hex(0);
      comps[1] = hex(1);
      comps[2] = hex(2);
      divider = 15;
      break;
    case 8:
      comps[3] = (hex(6) * 16 + hex(7));
      has_alpha = true;
      if(false)
    case 6:
        has_alpha = false;
      comps[0] = hex(0) * 16 + hex(1);
      comps[1] = hex(2) * 16 + hex(3);
      comps[2] = hex(4) * 16 + hex(5);
      divider = 255;
      break;
    case 16:
      comps[3] = hex(12)*4096 + hex(13)*256 + hex(14)*16 + hex(15);
      has_alpha = true;
      if(false)
    case 12:
        has_alpha = false;
      comps[0] = hex(0)*4096 + hex(1)*256 + hex(2)*16 + hex(3);
      comps[1] = hex(4)*4096 + hex(5)*256 + hex(6)*16 + hex(7);
      comps[2] = hex(8)*4096 + hex(9)*256 + hex(10)*16 + hex(11);
      divider = 65535;
      break;
    default:
      return 0;
  }
  if(error)
    return 0;
  return divider;
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
