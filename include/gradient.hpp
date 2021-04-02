#pragma once

#include "colorspace.hpp"

#include <map>
#include <array>

namespace cspace {
  template<int dimension>
  struct gradient {
    // A map sorts its keys automatically
    // we want the positions of the points (the keys) to be sorted
    std::map<double, std::array<double, dimension>> points;

    void convert(colorspace from, colorspace to, int offset = 0);
    void auto_add(size_t count);
    void get(double at, double output[dimension]) const;
    void add_hex(double at, const std::string&, bool alpha_first);
    std::string get_hex(double at) const;
  };
  template struct gradient<1>;
  template struct gradient<3>;
  template struct gradient<4>;
}
