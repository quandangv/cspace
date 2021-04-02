#include "gradient.hpp"
#include "parse.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace cspace {

template<int dimension>
void gradient<dimension>::convert(colorspace from, colorspace to, int offset) {
  for (auto& p : points)
    cspace::convert(p.second.data() + offset, from, to);
}

inline double linear(double from, double to, double interval) {
  return from + (to - from)*interval;
}

template<int dimension>
void gradient<dimension>::auto_add(size_t count) {
  if (points.size() < 2)
    throw std::logic_error("Can't automatically add to a gradient with less than 2 points");
  // std::map sorts its key, so we can assume this to be positive
  double range = points.rbegin()->first - points.begin()->first;
  double ideal_size = range / (points.size() + count - 1);
  range -= ideal_size * (points.size() - 1);
  size_t remaining = count;

  for (auto before = points.begin(), it = ++points.begin(); it != points.end(); ++it) {
    auto insert_count = std::min(
        (size_t)lround((it->first - before->first - ideal_size) / range * count), remaining);
    remaining -= insert_count;

    auto increment = 1.0 / (insert_count + 1);
    auto hint = before;
    for (auto pos = increment; insert_count--> 0; pos += increment) {
      // emplace for std::map doesn't invalidate other iterators
      hint = points.emplace_hint(hint, linear(before->first, it->first, pos), std::array<double, dimension>());
      for (int i = 0; i < dimension; i++)
        hint->second[i] = linear(before->second[i], it->second[i], pos);
    }
    before = it;
  }
}

template<int dimension>
void gradient<dimension>::get(double at, double output[dimension]) const {
  if (points.size() < 1)
    throw std::logic_error("Can't get from a gradient with less than 1 points");
  auto it = points.lower_bound(at);
  if (it == points.begin() || it->first == at)
    goto copy;
  if (it == points.end()) {
    it = --points.end();
    goto copy;
  } else {
    // interpolate
    auto& after = *it, before = *(--it);
    auto pos = (at - before.first) / (after.first - before.first);
    for (int i = 0; i < dimension; i++)
      output[i] = linear(before.second[i], after.second[i], pos);
    return;
  }
copy:
  for (int i = 0; i < dimension; i++)
    output[i] = it->second[i];
}

template<int dimension>
std::string gradient<dimension>::get_hex(double at) const {
  double tmp[dimension];
  get(at, tmp);
  std::stringstream ss;
  ss << std::ios::hex << '#';
  for(int i = 0; i < dimension; i++)
    ss << std::setw(2) << lround(tmp[i]*255);
  return ss.str();
}

template<int dimension>
void gradient<dimension>::add_hex(double at, const std::string& str, bool alpha_first) {
  if constexpr(dimension != 4)
    throw std::logic_error("Incompatible dimensions");
  bool alpha;
  auto data = points.emplace(at, std::array<double, dimension>()).first->second.data();
  if (str.empty() || str.front() != '#' || !parse_hex(tstring(str.data() + 1), data, alpha))
    throw std::logic_error("Invalid hex string: " + str);
  if (!alpha) {
    if (alpha_first) {
      data[3] = data[2];
      data[2] = data[1];
      data[1] = data[0];
      data[0] = 1;
    } else
      data[3] = 1;
  }
}

}
