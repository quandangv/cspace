#include "interface.hpp"

#include <cmath>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

// Start conversion using the parameters and return the resulting string
string interface::pop_data(double* data_ptr, colorspace from, colorspace to) {
  this->count = 0;
  if (comma) {
    data_cache = data;
    return "";
  } else if (!color_eater.empty()) {
    feed_color_eater();
    return "";
  }

  if (!modifications.empty()) {
    colorspaces::convert(data_ptr, from, inter);
    for(auto mod : modifications)
      mod.apply(data_ptr);
    colorspaces::convert(data_ptr, inter, to);
  } else colorspaces::convert(data_ptr, from, to);
  if (clamp || hex) colorspaces::clamp(data_ptr, to);
  
  // print data to output stream
  output_stream.str("");
  if (hex) {

    // Print hex code
    // std::hex have already been passed to output_stream when turning on hex.
    if (data.alpha != 1.0 && alpha_first) {
      output_stream << std::setw(2) << (int)round(data.alpha * 255.0);
      data.alpha = 1.0;
    }
    for(int i = 0, count = colorspaces::component_count(to); i < count; i++)
      output_stream << std::setw(2) << (int)round(data_ptr[i]*255);
    if (data.alpha != 1.0 && !alpha_first) {
      output_stream << std::setw(2) << (int)round(data.alpha * 255.0);
      data.alpha = 1.0;
    }
  } else {

    // Print decimal components
    if (data.alpha != 1.0 & alpha_first) {
      output_stream << data.alpha << separator;
      data.alpha = 1.0;
    }
    output_stream << data_ptr[0];
    for(int i = 1, count = colorspaces::component_count(to); i < count; i++)
      output_stream << separator << data_ptr[i];
    if (data.alpha != 1.0 & !alpha_first) {
      output_stream << separator << data.alpha;
      data.alpha = 1.0;
    }
  }
  return output_stream.str();
}
