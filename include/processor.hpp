#pragma once

#include <vector>
#include <sstream>
#include <ostream>
#include <cmath>
#include <stdexcept>

#include "colorspace.hpp"
#include "error.hpp"

namespace cspace {

  struct mod {
    struct error : error_base { using error_base::error_base; };

    int component;
    char op{0};
    double value{std::nan("")};

    mod() {}
    mod(int component, char op, double value) : component(component), op(op), value(value) {}
    mod(const std::string&, colorspace);

    void apply(double* target) const;
  };

  class processor {
  protected:
    colorspace m_target{colorspaces::rgb};
    void print(std::ostream&, double* data, int count) const;

  public:
    struct error : error_base { using error_base::error_base; };

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
    mod& add_modification(const std::string&);
    void silent_operate(const std::string&) const;
    void silent_operate(double* data, bool have_alpha, colorspace from) const;

    template<typename... Args> std::string operate(Args... args) const {
      output_stream.str("");
      silent_operate(args...);
      return output_stream.str();
    }
  };
}
