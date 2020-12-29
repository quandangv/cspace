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

    mod() {}
    mod(int component, char op, double value) : component(component), op(op), value(value) {}
    mod(token_iterator&, colorspace);
    mod(std::string&&, colorspace);

    void apply(double* target) const;
  };

  class processor {
    colorspace m_target{colorspaces::rgb};
  public:
    colorspace inter{colorspaces::cielab};
    std::string separator{" "};
    std::vector<mod> modifications{};
    bool alpha_first{true};
    mutable std::stringstream output_stream;

    processor();

    bool use_hex();
    bool use_hex(bool);
    colorspace target() const;
    colorspace target(colorspace);
    mod& add_modification(std::string&&);
    std::string operate_hex(const std::string&);
    std::string operate(double* data, bool have_alpha, colorspace from);
    std::string print(double*data, int count) const;
  };
}