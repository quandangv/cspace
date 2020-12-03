#pragma once

#include "error.hpp"

void rgb_hsl(const double* input, double* output); 
void hsl_rgb(const double* input, double* output); 
void rgb_hsv(const double* input, double* output); 
void hsv_rgb(const double* input, double* output); 
void xyz_rgb(const double* input, double* output, double white_lum = 1); 
void rgb_xyz(const double* input, double* output, double white_lum = 1); 
void xyz_jzazbz(const double* input, double* output); 
void jzazbz_xyz(const double* input, double* output); 
void xyz_cielab(const double* input, double* output); 
void cielab_xyz(const double* input, double* output); 
void ab_ch(const double* input, double* output); 
void ch_ab(const double* input, double* output); 

DEFINE_ERROR(conversion_error)

using colorspace = unsigned char;
namespace colorspaces {
  constexpr colorspace xyz          = 0b00000000;

  constexpr colorspace xyz_msk      = 0b00000011;
  constexpr colorspace rgb          = 0b00000001;
  constexpr colorspace jzazbz       = 0b00000010;
  constexpr colorspace cielab       = 0b00000011;

  constexpr colorspace rgb_msk      = 0b00001111;
  constexpr colorspace hsl          = 0b00000101;
  constexpr colorspace hsv          = 0b00001001;

  constexpr colorspace jzazbz_msk   = 0b00000111;
  constexpr colorspace jzczhz       = 0b00000110;

  constexpr colorspace cielab_msk   = 0b00000111;
  constexpr colorspace cielch       = 0b00000111;
}

void convert(double* value, colorspace from, colorspace to);

