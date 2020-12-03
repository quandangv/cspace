#include "conversion.hpp"

using namespace std;

colorspace get_mask(colorspace c) {
#define ENTRY(color) if (c == colorspaces::color) return colorspaces::color##_msk;
  ENTRY(xyz)
  ENTRY(rgb)
  ENTRY(jzazbz)
  ENTRY(cielab)
#undef ENTRY
  return 0;
}

void direct(double* value, colorspace from, colorspace to) {
  unsigned short combined = from << 8 | to;
  switch (combined) {
#define CASE(a, b) \
  case colorspaces::a << 8 | colorspaces::b: a##_##b(value, value); break; \
  case colorspaces::b << 8 | colorspaces::a: b##_##a(value, value); break;
#define AB_CH(ab, ch) \
  case colorspaces::ab << 8 | colorspaces::ch: ab_ch(value, value); break; \
  case colorspaces::ch << 8 | colorspaces::ab: ch_ab(value, value); break;
    CASE(rgb, xyz)
    CASE(rgb, hsl)
    CASE(rgb, hsv)
    CASE(xyz, jzazbz)
    AB_CH(jzazbz, jzczhz)
    AB_CH(cielab, cielch)
    default: throw conversion_error("Color-Direct: Unknown color conversion: from " + to_string(from) + " to " + to_string(to)); 
#undef AB_CH
#undef CASE
  }
}

void convert_up(double* value, colorspace from, colorspace parent) {
  if (from == parent) return;
  auto next_mask = from & get_mask(parent);
  convert_up(value, from, next_mask);
  direct(value, next_mask, parent);
}

void convert_down(double* value, colorspace parent, colorspace to) {
  if (parent == to) return;
  auto next_mask = to & get_mask(parent);
  convert_down(value, next_mask, to);
  direct(value, parent, next_mask);
}
  
void convert(double* value, colorspace from, colorspace to) {
  if (from == to) return;
  auto parent = colorspaces::xyz;
  auto common = from & get_mask(parent);
  while (common == to & get_mask(parent)) {
    parent = common;
  }
  convert_up(value, from, parent);
  convert_down(value, parent, to);
}

