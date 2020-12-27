#pragma once
#include "common.h"

GLOBAL_NAMESPACE

void rgb_hsl(const double*, double*); 
void hsl_rgb(const double*, double*); 
void rgb_hsv(const double*, double*); 
void hsv_rgb(const double*, double*); 
void xyz_rgb(const double*, double*, double white_lum = 1); 
void rgb_xyz(const double*, double*, double white_lum = 1); 
void xyz_jzazbz(const double*, double*); 
void jzazbz_xyz(const double*, double*); 
void xyz_cielab(const double*, double*); 
void cielab_xyz(const double*, double*); 
void ab_ch(const double*, double*); 
void ch_ab(const double*, double*); 
void rgb_cmyk(const double*, double*);
void cmyk_rgb(const double*, double*);

GLOBAL_NAMESPACE_END
