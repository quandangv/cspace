#include "conversion.hpp"

#include <cmath>

void ab_ch(const double* i, double* o) {
  auto h = atan2(i[2], i[1]);
  h = h > 0 ? (h / M_PI) * 180 : 360 + h / M_PI * 180;
  o[1] = sqrt(i[1] * i[1] + i[2] * i[2]);
  o[2] = h;
  o[0] = i[0];
}

void ch_ab(const double* i, double* o) {
  auto h = i[2] / 180 * M_PI;
  auto chroma = i[1];
  o[1] = cos(h) * chroma;
  o[2] = sin(h) * chroma;
  o[0] = i[0];
}
