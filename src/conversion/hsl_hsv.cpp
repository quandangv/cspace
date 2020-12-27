#include "conversion.hpp"

#include <cmath>

GLOBAL_NAMESPACE

// source: https://en.wikipedia.org/wiki/HSL_and_HSV

template<bool l>
void rgb_x(const double* input, double* output) {
  double r = input[0], g = input[1], b = input[2];
  double max = fmax(r, fmax(g, b)),
  			 min = fmin(r, fmin(g, b));
  if constexpr(l)
    output[2] = (max + min) / 2;
  else output[2] = max;

  if (max == min) {
    output[0] = output[1] = 0;
  } else {
    auto d = max - min;
    if constexpr(l)
      output[1] = (max - output[2]) / fmin(output[2], 1 - output[2]);
    else output[1] = d / max;
    if (max == r)      output[0] = (g - b) / d + (g < b ? 6 : 0);
    else if (max == g) output[0] = (b - r) / d + 2;
    else               output[0] = (r - g) / d + 4;
    output[0] *= 60;
  }
}

void rgb_hsl(const double* input, double* output) {
  rgb_x<true>(input, output);
}

void rgb_hsv(const double* input, double* output) {
  rgb_x<false>(input, output);
}

void hsl_rgb(const double* input, double* output) {
  auto h = input[0] / 30.0,
       a = input[1] * fmin(input[2], 1 - input[2]);
  auto f = [=](int n) {
    auto k = fmod(n + h, 12);
    return input[2] - a*fmax(-1, fmin(1, fmin(k-3, 9-k)));
  };
  output[0] = f(0);
  output[1] = f(8);
  output[2] = f(4);
}

void hsv_rgb(const double* input, double* output) {
  auto h = input[0] / 60.0,
       a = input[1] * input[2];
  auto f = [=](int n) {
    auto k = fmod(n + h, 6);
    return input[2] - a*fmax(0, fmin(1, fmin(k, 4-k)));
  };
  output[0] = f(5);
  output[1] = f(3);
  output[2] = f(1);
}

GLOBAL_NAMESPACE_END
