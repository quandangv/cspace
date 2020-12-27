#include "conversion.hpp"

#include <cmath>

GLOBAL_NAMESPACE

// source: http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html

inline double gamma(double x) {
  return x > 0.0031308 ? 1.055 * pow(x, 1/2.4) - 0.055 : 12.92 * x;
}
inline double inverse_gamma(double x) {
  return x > 0.04045 ? pow((x + 0.055) / 1.055, 2.4) : x / 12.92;
}

void xyz_rgb(const double* i, double* o, double white_lum) {
  double x = i[0], y = i[1], z = i[2];
  o[0] = gamma( 3.240454836021408466100*x - 1.53713885010257506520*y - 0.498531546868480873440*z) / white_lum;
  o[1] = gamma(-0.969266389875653771680*x + 1.87601092884249122020*y + 0.041556082346673526519*z) / white_lum;
  o[2] = gamma( 0.055643419604213656667*x - 0.20402585426769813732*y + 1.057225162457928795000*z) / white_lum;
}

void rgb_xyz(const double* i, double* o, double white_lum) {
  double r = inverse_gamma(i[0]) * white_lum, g = inverse_gamma(i[1]) * white_lum, b = inverse_gamma(i[2]) * white_lum;
  o[0] = 0.4124564*r + 0.3575761*g +  0.1804375*b;
  o[1] = 0.2126729*r + 0.7151522*g +  0.0721750*b;
  o[2] = 0.0193339*r + 0.1191920*g +  0.9503041*b;
}

GLOBAL_NAMESPACE_END
