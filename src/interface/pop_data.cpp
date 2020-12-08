#include "interface.hpp"

#include <cmath>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

// Start conversion using the parameters and return the resulting string
string interface::pop_data(double* data, colorspace from, colorspace to) {
  this->count = 0;
  colorspaces::convert(data, from, to);
  if (clamp || hex) colorspaces::clamp(data, to);
  
  // print data to output stream
  output_stream.str("");
  if (hex) {

    // Print hex code
    // std::hex have already been passed to output_stream when turning on hex.
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

    // Print decimal components
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
