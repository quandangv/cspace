#include "conversion.hpp"

#include <cmath>

// source: http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html

inline double gamma(double x) {
  return x > 0.0031308 ? 1.055 * pow(x, 1/2.4) - 0.055 : 12.92 * x;
}
inline double inverse_gamma(double x) {
  return x > 0.04045 ? pow((x + 0.055) / 1.055, 2.4) : x / 12.92;
}

void xyz_rgb(const double* i, double* o, double white_lum) {
  double x = i[0], y = i[1], z = i[2];
  o[0] = gamma( 3.2404542*x - 1.5371385*y - 0.4985314*z) / white_lum;
  o[1] = gamma(-0.9692660*x + 1.8760108*y + 0.0415560*z) / white_lum;
  o[2] = gamma( 0.0556434*x - 0.2040259*y + 1.0572252*z) / white_lum;
}

void rgb_xyz(const double* i, double* o, double white_lum) {
  double r = inverse_gamma(i[0]) * white_lum, g = inverse_gamma(i[1]) * white_lum, b = inverse_gamma(i[2]) * white_lum;
  o[0] = 0.4124564*r + 0.3575761*g +  0.1804375*b;
  o[1] = 0.2126729*r + 0.7151522*g +  0.0721750*b;
  o[2] = 0.0193339*r + 0.1191920*g +  0.9503041*b;
}
