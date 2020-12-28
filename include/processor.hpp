#pragma once

#include <vector>
#include <sstream>
#include <cmath>

#include "token_iterator.hpp"
#include "colorspace.hpp"

namespace cspace {
  struct mod {
    int component;
    char op{0};
    double value{std::nan("")};

    mod(int component, char op, double value) : component(component), op(op), value(value) {}
    mod(token_iterator&, colorspace);

    void apply(double* target) const;
  };

  class processor {
  public:
    colorspace target{colorspaces::rgb};
    colorspace inter{colorspaces::jzazbz};
    std::string separator{" "};
    std::vector<mod> modifications{};
    bool alpha_first{true};
    std::stringstream output_stream;

    processor();

    bool use_hex();
    bool use_hex(bool);
    std::string operate(double* data, bool have_alpha, colorspace from);
    std::string get_state();
    void clear();
  };
}
