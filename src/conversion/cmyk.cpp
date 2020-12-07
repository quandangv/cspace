#include "conversion.hpp"

#include <cmath>

// this is added mainly to make sure we are capable of working with 4-component colorspaces

void rgb_cmyk(const double* input, double* output) {
  auto max = fmax(input[0], fmax(input[1], input[2]));
  if (max == 0) {
    output[0] = output[1] = output[2] = 0;
    output[3] = 1;
  } else {
    output[0] = (max - input[0]) / max;
    output[1] = (max - input[1]) / max;
    output[2] = (max - input[2]) / max;
    output[3] = 1 - max;
  }
}

void cmyk_rgb(const double* input, double* output) {
  output[0] = 1 - input[0] - input[3];
  output[1] = 1 - input[1] - input[3];
  output[2] = 1 - input[2] - input[3];
}
