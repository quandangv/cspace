#include "conversion.hpp"

#include <cmath>

// source: https://en.wikipedia.org/wiki/CIELAB_color_space

constexpr double Xn = 0.950489;
constexpr double Yn = 1;
constexpr double Zn = 1.08884;
constexpr double delta = 6.0 / 29.0;

inline double f(double input) {
  if (input > delta*delta*delta)
    return std::cbrt(input);
  return input / (3*delta*delta) + 4.0 / 29.0;
}

inline double inverse_f(double input) {
  if (input > delta)
    return input*input*input;
  return 3*delta*delta * (input - 4.0/29.0);
}

void xyz_cielab(const double* input, double* output) {
  double yp = f(input[1] / Yn);
  output[1] = 500 * (f(input[0] / Xn) - yp);
  output[2] = 200 * (yp - f(input[2] / Zn));
  output[0] = 116.0*yp - 16.0;
}

void cielab_xyz(const double* input, double* output) {
  double lp = (input[0] + 16) / 116.0;
  output[0] = Xn * inverse_f(lp + input[1] / 500);
  output[1] = Yn * inverse_f(lp);
  output[2] = Zn * inverse_f(lp - input[2] / 200);
}

