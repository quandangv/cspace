#pragma once

#include <string>

namespace cspace {
  using colorspace = unsigned char;

  std::string spacetos(colorspace);
  colorspace stospace(const std::string&);
  std::string list_colorspaces(const std::string& separator);

  void convert(double* value, colorspace from, colorspace to);
  int component_count(colorspace);
  void clamp(double* value, colorspace);

  namespace colorspaces {
    // All colorspaces are organized to a tree
    // The colorspace of value 0 is the root
    constexpr colorspace xyz          = 0b00000000;

    // The mask of the root marks the bits to denote direct children of the root
    // The higher bits are for denoting descendant of each child
    constexpr colorspace xyz_msk      = 0b00000011;
    constexpr colorspace rgb          = 0b00000001;
    constexpr colorspace jzazbz       = 0b00000010;
    constexpr colorspace cielab       = 0b00000011;

    // The mask of a node marks the bits to denote its direct children
    constexpr colorspace rgb_msk      = 0b00001111;
    constexpr colorspace hsl          = 0b00000101;
    constexpr colorspace hsv          = 0b00001001;
    constexpr colorspace cmyk         = 0b00001101;

    constexpr colorspace jzazbz_msk   = 0b00000111;
    constexpr colorspace jzczhz       = 0b00000110;

    constexpr colorspace cielab_msk   = 0b00000111;
    constexpr colorspace cielch       = 0b00000111;
    // The lower bits are used first to make easier for later expansion
  }
}
