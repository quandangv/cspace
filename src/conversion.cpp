#include "conversion.hpp"

#include <cmath>

void rgb_hsv(const double* input, double* output) {
}

void hsv_rgb(const double* input, double* output) {
}

// CIE Lab - XYZ {{{
constexpr double Xn = 0.950489;
constexpr double Yn = 1;
constexpr double Zn = 1.08884;
constexpr double delta = 6.0 / 29.0;

inline double cielab_f(double input) {
  if (input > delta*delta*delta)
    return cbrt(input);
  return input / (3*delta*delta) + 4.0 / 29.0;
}

inline double inverse_cielab_f(double input) {
  if (input > delta)
    return input*input*input;
  return 3*delta*delta * (input - 4.0/29.0);
}

void xyz_cielab(const double* input, double* output) {
  double yp = cielab_f(input[1] / Yn);
  output[1] = 500 * (cielab_f(input[0] / Xn) - yp);
  output[2] = 200 * (yp - cielab_f(input[2] / Zn));
  output[0] = 116.0*yp - 16.0;
}

void cielab_xyz(const double* input, double* output) {
  double lp = (input[0] + 16) / 116.0;
  output[0] = Xn * inverse_cielab_f(lp + input[1] / 500);
  output[1] = Yn * inverse_cielab_f(lp);
  output[2] = Zn * inverse_cielab_f(lp - input[2] / 200);
}
// }}}

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

// source: https://observablehq.com/@jrus/jzazbz

// RGB - XYZ {{{
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
// }}}

// RGB - Jzazbz {{{
// source: https://www.osapublishing.org/oe/fulltext.cfm?uri=oe-25-13-15131&id=368272
constexpr double c1 = 0xD60p-12;
constexpr double c2 = 0x96Dp-7;
constexpr double c3 = 0x958p-7;
constexpr double p = 1.7*0x9DBp-5;
constexpr double n = 0xA32p-14;

inline double perceptual_quantizer(double x) {
  double xx = pow(x/1e4, n);
  return pow((c1 + c2*xx) / (1 + c3*xx), p);
}
inline double inv_perceptual_quantizer(double l) {
  double ll = pow(l, 1/p);
  return 1e4 * pow((c1 - ll) / (c3*ll - c2), 1/n);
}

constexpr double d0 = 1.6295499532821566e-11;

void xyz_jzazbz(const double* i, double* o) {
  double xp = 1.15*i[0] - 0.15*i[2];
  double yp = 0.66*i[1] + 0.34*i[0];
  double lp = perceptual_quantizer(0.41478972*xp + 0.579999*yp + 0.0146480*i[2]);
  double mp = perceptual_quantizer(-0.2015100*xp + 1.120649*yp + 0.0531008*i[2]);
  double sp = perceptual_quantizer(-0.0166008*xp + 0.264800*yp + 0.6684799*i[2]);
  double iz = 0.5 * (lp + mp);
  o[0] = (0.44 * iz) / (1 - 0.56*iz) - d0;
  o[1] = 3.524000*lp - 4.066708*mp + 0.542708*sp;
  o[2] = 0.199076*lp + 1.096799*mp - 1.295875*sp;
}

void jzazbz_xyz(const double* i, double* o) {
  double jp = i[0] + 1.6295499532821566e-11;
  double iz = jp / (0.44 + 0.56*jp);
  double l = inv_perceptual_quantizer(iz + 0.255917185920468419580*i[1] + 0.49301692226219780071*i[2]);
  double m = inv_perceptual_quantizer(iz - 0.021292900622610172173*i[1] + 0.37692228994996004914*i[2]);
  double s = inv_perceptual_quantizer(iz + 0.021292900622610172173*i[1] - 0.37692228994996004914*i[2]);
  double xp =  1.924226435787606738300*l - 1.00479231259536551470*m + 0.037651404030618006045*s;
  double yp =  0.350316762094999082580*l + 0.72648119393165526698*m - 0.065384422948085030773*s;
  o[2]      = -0.090982810982847584683*l - 0.31272829052307400495*m + 1.522766561305260486600*s;
  o[0] = (xp + 0.15*o[2]) / 1.15;
  o[1] = (yp - 0.34*o[0]) / 0.66;
}
// }}}

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
