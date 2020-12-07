#include "parse.hpp"

#include <sstream>
#include <iomanip>

#include "logger.hpp"

using namespace std;

string to_string(const double* arr, colorspace space) {
  return to_string(arr, colorspaces::component_count(space));
}

string to_string(const double* arr, int count) {
  stringstream result;
  result  << arr[0];
  for(int i = 1; i < count; i++)
    result << ' ' << arr[i];
  return result.str();
}

template<bool from_str>
void str_space(string& str, colorspace& space) {
#define CASE(a) \
  if constexpr(from_str) { \
    if (strcasecmp(str.c_str(), #a) == 0) { \
      space = colorspaces::a; \
      return; \
    } \
  } else { \
    if (space == colorspaces::a) { \
      str = #a; \
      return; \
    } \
  }
  CASE(xyz);
  CASE(rgb);
  CASE(jzazbz);
  CASE(cielab);
  CASE(hsl);
  CASE(hsv);
  CASE(jzczhz);
  CASE(cielch);
#undef CASE
  if constexpr(from_str)
    throw parse_error("Unknown colorspace: '"s + str + "'");
  throw parse_error("Unknown colorspace: "s + to_string(space));
}

colorspace stospace(string&& value) {
  colorspace result;
  str_space<true>(value, result);
  return result;
}

string to_string(colorspace value) {
  string result;
  str_space<false>(result, value);
  return move(result);
}

int parse_code(const string& value, component& a, component& r,
               component& g, component& b, bool alpha_first) {
  bool error = false;
  int divider;
  auto str = value.c_str();
	auto hex = [&] (int index)->unsigned int {
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
  	case 4:
    	if (alpha_first) {
      	a = hex(0);
      	str++;
    	} else if (true) {
      	a = hex(3);
    	} else {
		case 3:
  			a = 15;
    	}
  		r = hex(0);
  		g = hex(1);
  		b = hex(2);
  		divider = 15;
			break;
		case 8:
  		if (alpha_first) {
  			a = (hex(0) * 16 + hex(1));
  			str += 2;
  		} else if (true) {
  			a = (hex(6) * 16 + hex(7));
  		} else {
		case 6:
    		a = 255;
			}
			r = hex(0) * 16 + hex(1);
			g = hex(2) * 16 + hex(3);
			b = hex(4) * 16 + hex(5);
			divider = 255;
			break;
		case 16:
  		if (alpha_first) {
  			a = hex(0)*4096 + hex(1)*256 + hex(2)*16 + hex(3);
  			str += 4;
  		} else if (true) {
  			a = hex(12)*4096 + hex(13)*256 + hex(14)*16 + hex(15);
  		} else {
		case 12:
  			a = 65535;
			}
			r = hex(0)*4096 + hex(1)*256 + hex(2)*16 + hex(3);
			g = hex(4)*4096 + hex(5)*256 + hex(6)*16 + hex(7);
			b = hex(8)*4096 + hex(9)*256 + hex(10)*16 + hex(11);
			divider = 65535;
			break;
		default:
  		return 0;
	}
  if(error)
    return 0;
  if (a == 0)
    r = g = b = 0;
  return divider;
}
