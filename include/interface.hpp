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

struct color_data {
  double data[5];
  double alpha{1.0};
  int count;

  void extract_alpha(bool alpha_first);
  double& current() { return data[count]; }
  double& operator[](int);
};

class interface {
public:
  color_data data, data_cache;
  colorspace from{colorspaces::rgb};
  colorspace to{colorspaces::rgb};
  bool clamp{false};
  bool quit{false};
  bool stay{false};
  bool hex{false};
  bool comma;
  std::string separator{" "};

  // Support for alpha component
  bool alpha_first{true};
  bool alpha{false};

  colorspace inter{colorspaces::jzazbz};
  std::vector<mod> modifications;

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
  std::string pop_data(double*, colorspace, colorspace);
};

