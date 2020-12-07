#include "interface.hpp"

#include <cmath>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

string interface::pop_data(colorspace from, colorspace to) {
  this->count = 0;
  // Use the parsed term as data for color conversion
  colorspaces::convert(&data[0], from, to);
  if (clamp || hex) colorspaces::clamp(&data[0], to);
  
  // print data to output stream
  output_stream.str("");
  if (hex) {

    // print hex code
    // std::hex have already been passed to output_stream in the hex! term
    if (alpha_val != 1.0 && alpha_first) {
      output_stream << std::setw(2) << (int)round(alpha_val * 255.0);
      alpha_val = 1.0;
    }
    for(int i = 0, count = colorspaces::component_count(to); i < count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
    if (alpha_val != 1.0 && !alpha_first) {
      output_stream << std::setw(2) << (int)round(alpha_val * 255.0);
      alpha_val = 1.0;
    }
  } else {
    if (alpha_val != 1.0 & alpha_first) {
      output_stream << alpha_val << separator;
      alpha_val = 1.0;
    }
    output_stream << data[0];
    for(int i = 1, count = colorspaces::component_count(to); i < count; i++)
      output_stream << separator << data[i];
    if (alpha_val != 1.0 & !alpha_first) {
      output_stream << separator << alpha_val;
      alpha_val = 1.0;
    }
  }
  return output_stream.str();
}
