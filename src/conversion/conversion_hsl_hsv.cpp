#include "conversion.hpp"

#include <cmath>

// source: https://en.wikipedia.org/wiki/HSL_and_HSV

void rgb_hsl(const double* input, double* output) {
  double r = input[0], g = input[1], b = input[2];
  double max = fmax(r, fmax(g, b)),
  			 min = fmin(r, fmin(g, b));
  output[2] = (max + min) / 2;

  if (max == min) {
    output[0] = output[1] = 0;
  } else {
    auto d = max - min;
    output[1] = d / (output[2] > 0.5 ? 2 - max - min : max + min);
    if (max == r)      output[0] = (g - b) / d + (g < b ? 6 : 0);
    else if (max == g) output[0] = (b - r) / d + 2;
    else               output[0] = (r - g) / d + 4;
    output[0] *= 60;
  }
}

inline double hue2rgb(double p, double q, double t) {
  if (t > 360.0) t -= 360.0;
  if (t < 60.0) return p + (q - p) * t/60.0;
  if (t < 180.0) return q;
  if (t < 240.0) return p + (q - p) * (240.0 - t)/60.0;
  return p;
}

void hsl_rgb(const double* input, double* output) {
  double h = input[0];
	const double& s = input[1], l = input[2];
	if (s == 0.0) {
    output[0] = output[1] = output[2] = l;
  } else {
    auto q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
    auto p = 2.0 * l - q;
    output[0] = hue2rgb(p, q, h + 120.0);
    output[1] = hue2rgb(p, q, h);
    output[2] = hue2rgb(p, q, h + 240.0);
  }
}

void rgb_hsv(const double* input, double* output) {
}

void hsv_rgb(const double* input, double* output) {
}
