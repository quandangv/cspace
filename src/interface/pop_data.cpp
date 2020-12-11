#include "interface.hpp"

#include <cmath>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

// Start conversion using the parameters and return the resulting string
string interface::pop_data(colorspace from, colorspace to) {
  output_stream.str("");
  auto data_ptr = &data[(int)(alpha && alpha_first)];
  if (!modifications.empty()) {
    colorspaces::convert(data_ptr, from, inter);
    for(auto mod : modifications)
      mod.apply(data_ptr);
    colorspaces::convert(data_ptr, inter, to);
  } else
    colorspaces::convert(data_ptr, from, to);
  colorspaces::clamp(data_ptr, to);
  
  // print data to output stream
  auto comp_count = colorspaces::component_count(to) + (int)alpha;
  if (output_stream.flags() & std::ios::hex) {
    for(int i = 0; i < comp_count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
  } else {
    output_stream << data[0];
    for(int i = 1; i < comp_count; i++)
      output_stream << separator << data[i];
  }
  data_count = 0;
  alpha = false;
  return output_stream.str();
}
