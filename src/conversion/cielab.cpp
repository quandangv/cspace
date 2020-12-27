#include "conversion.hpp"

#include <cmath>

GLOBAL_NAMESPACE

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

inline double inv_f(double input) {
  if (input > delta)
    return input*input*input;
  return 3*delta*delta * (input - 4.0/29.0);
}

void xyz_cielab(const double* input, double* output) {
  double yp = f(input[1] / Yn);
  output[1] = 5 * (f(input[0] / Xn) - yp);
  output[2] = 2 * (yp - f(input[2] / Zn));
  output[0] = 1.16*yp - 0.16;
}

void cielab_xyz(const double* input, double* output) {
  double lp = (input[0] + 0.16) / 1.16;
  output[0] = Xn * inv_f(lp + input[1] / 5);
  output[1] = Yn * inv_f(lp);
  output[2] = Zn * inv_f(lp - input[2] / 2);
}

GLOBAL_NAMESPACE_END
