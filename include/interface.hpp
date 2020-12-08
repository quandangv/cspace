#include <iostream>
#include <sstream>
#include <vector>

#include "conversion.hpp"
#include "parse.hpp"
#include "logger.hpp"
#include "error.hpp"

DEFINE_ERROR(interface_error);

class interface {
public:
  double data[5];
  int count{0};
  colorspace from{colorspaces::rgb};
  colorspace to{colorspaces::rgb};
  bool clamp{false};
  bool quit{false};
  bool stay{false};
  std::string separator{" "};

  // Support for hexedecimal color code and alpha component
  bool hex{false};
  bool alpha_first{true};
  bool alpha{false};
  double alpha_val{1.0};

  std::string add_term(std::string&&);
  std::string get_state() const;
  void clear();
  void makesure_empty() const;
  
private:
  std::stringstream output_stream;
  std::vector<std::string> waiting_terms;
  
  void print_help();
  void process_short_switches(const std::string& switches);
  bool process_long_switch(const std::string& name);
  template<bool, bool> bool switches(const std::string&);

  void feed_waiting_term(const std::string& term, std::string&& data);
  std::string pop_data(double*, colorspace, colorspace);
};

