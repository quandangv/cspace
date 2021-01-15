#pragma once

#include "processor.hpp"

namespace cspace {
  struct term_eater;

  class interface : public processor {
    const term_eater* eater{nullptr};
    bool comma{false};
    bool alpha{false};

    void process_short_switches(const std::string& switches);
    bool process_long_switch(const std::string& name);
    void feed_term_eater(std::string&& data);
    void unexpected_comma(const std::string& term);
    bool pop_data();
    bool pop_data(colorspace);
    bool add_data(double);

  public:
    struct error : error_base { using error_base::error_base; };

    double data[5];
    int data_count{0};
    colorspace from{colorspaces::rgb};
    bool quit{false};
    bool stay{false};

    void makesure_empty();
    void add_term_eater(const term_eater*);
    bool silent_add_term(const std::string&, const std::string& separator);
    bool silent_add_term(std::string&&);

    template<typename... Args> std::string add_term(Args... args) {
      output_stream.str("");
      silent_add_term(args...);
      return output_stream.str();
    }
  };
}
