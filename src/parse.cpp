#include "parse.hpp"

#include <sstream>
#include <iomanip>

#include "logger.hpp"

using namespace std;

// Convert the right number components of the given colorspace to string
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

// Parse the color code and set the value to each component
// Returns the maximum value; ie. 15 for 4-bit colors, 255 for 8-bit colors...
// alpha_first informs whether the color code is in the [A]RGB or RGB[A] format
int parse_code(const string& value, component& a, component& r,
               component& g, component& b, bool alpha_first) {
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
    	if (alpha_first) {
      	a = hex(0);
      	str++;
    	} else if (true) {
      	a = hex(3);
    	} else {
		case 3:
  		  // This will set alpha to maximum,
  		  // but will be skipped if we fall throughfrom the previous case
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
  return divider;
}

bool parse(string str, double& result) {
  int len;
  return sscanf(str.c_str(), "%lf%n", &result, &len) == 1 && len == str.size();
}

bool parse(string str, int& result) {
  int len;
  return sscanf(str.c_str(), "%d%n", &result, &len) == 1 && len == str.size();
}

bool parse(string str, bool& result) {
  auto c_str = str.c_str();
  #define CMP(a) strcasecmp(c_str, #a) == 0
  if (CMP(on) || CMP(true))
    result = true;
  else if (CMP(off) || CMP(false))
    result = false;
  else return false;
  return true;
  #undef CMP
}
