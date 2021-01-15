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

  public:
    struct error : error_base { using error_base::error_base; };

    double data[5];
    int data_count{0};
    colorspace from{colorspaces::rgb};
    bool quit{false};
    bool stay{false};

    std::string pop_data();
    std::string pop_data(colorspace);
    std::string add_term(std::string&&);
    std::string add_data(double);
    std::string add_multiple_terms(const std::string&, const std::string& separator = "\n");
    void makesure_empty();
    void add_term_eater(const term_eater*);
  };
}
