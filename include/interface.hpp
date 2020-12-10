#include <iostream>
#include <sstream>
#include <vector>

#include "conversion.hpp"
#include "parse.hpp"
#include "logger.hpp"
#include "error.hpp"

DEFINE_ERROR(interface_error);

struct mod {
  int component;
  char op;
  double value;

  void apply(double* target) const;
};

class interface {
public:
  double data[5], data_cache[5];
  int data_count;
  colorspace from{colorspaces::rgb};
  colorspace to{colorspaces::rgb};
  colorspace inter{colorspaces::jzazbz};
  std::string separator{" "};
  std::vector<mod> modifications;
  bool clamp{false};
  bool quit{false};
  bool stay{false};
  bool hex{false};
  bool comma{false};
  bool alpha_first{true};
  bool alpha{false};


  std::string add_term(std::string&&);
  std::string get_state();
  void clear();
  void makesure_empty();
  
private:
  std::stringstream output_stream;
  std::string term_eater;
  std::string color_eater;
  
  void print_help();
  void process_short_switches(const std::string& switches);
  bool process_long_switch(const std::string& name);
  template<bool, bool> bool switches(const std::string&);

  void add_term_eater(std::string&&);
  void add_color_eater(std::string&&);
  void feed_term_eater(std::string&& data);
  void feed_color_eater();
  void unexpected_comma(const std::string& term);
  std::string pop_data(colorspace, colorspace);
};

