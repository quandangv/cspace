#include "interface.hpp"

#include <cmath>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

// Start conversion using the parameters and return the resulting string
string interface::pop_data(colorspace from, colorspace to, bool include_alpha) {
  data_count = 0;
  if (comma) {
    for (int i = 0; i < 5; i++)
      data_cache[i] = data[i];
    return "";
  } else if (!color_eater.empty()) {
    feed_color_eater();
    return "";
  }

  auto data_ptr = &data[(int)(include_alpha && alpha_first)];
  if (!modifications.empty()) {
    colorspaces::convert(data_ptr, from, inter);
    for(auto mod : modifications)
      mod.apply(data_ptr);
    colorspaces::convert(data_ptr, inter, to);
  } else
    colorspaces::convert(data_ptr, from, to);
  if (clamp || hex) colorspaces::clamp(data_ptr, to);
  
  // print data to output stream
  output_stream.str("");
  auto comp_count = colorspaces::component_count(to) + (int)include_alpha;
  if (hex) {

    // Print hex code
    // std::hex have already been passed to output_stream when turning on hex.
    for(int i = 0; i < comp_count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
  } else {

    // Print decimal components
    output_stream << data[0];
    for(int i = 1; i < comp_count; i++)
      output_stream << separator << data[i];
  }
  return output_stream.str();
}
