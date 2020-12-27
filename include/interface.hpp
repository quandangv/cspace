#pragma once
#include "common.h"

#include "processor.hpp"

GLOBAL_NAMESPACE

DEFINE_ERROR(interface_error);

struct term_eater;

class interface : public processor {
public:
  double data[5];
  int data_count{0};
  colorspace from{colorspaces::rgb};
  bool quit{false};
  bool stay{false};

  std::string pop_data();
  std::string pop_data(colorspace);
  std::string add_term(std::string&&);
  void makesure_empty();
  void print_help();
  void add_term_eater(const term_eater*);
  
protected:
  const term_eater* eater{nullptr};
  bool comma{false};
  bool alpha{false};
  
  void process_short_switches(const std::string& switches);
  bool process_long_switch(const std::string& name);
  template<bool, bool> bool switches(const std::string&);

  void feed_term_eater(std::string&& data);
  void unexpected_comma(const std::string& term);
};

GLOBAL_NAMESPACE_END
