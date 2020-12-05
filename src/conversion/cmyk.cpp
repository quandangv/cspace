#include "conversion.hpp"

#include <cmath>

// this is added mainly to make sure we are capable of working with 4-component colorspaces

void rgb_cmyk(const double* input, double* output) {
  auto black = fmax(input[0], fmax(input[1], input[2]));
  if (black == 0)
    output[0] = output[1] = output[2] = output[3] = 0;
  else {
    output[0] = (black - input[0]) / black;
    output[1] = (black - input[1]) / black;
    output[2] = (black - input[2]) / black;
    output[3] = 1 - black;
  }
}

void cmyk_rgb(const double* input, double* output) {
  output[0] = 1 - input[0] - input[3];
  output[1] = 1 - input[1] - input[3];
  output[2] = 1 - input[2] - input[3];
}
