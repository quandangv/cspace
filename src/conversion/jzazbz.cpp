#include "conversion.hpp"

#include <cmath>

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
