#include <iostream>
#include <sstream>
#include <vector>

#include "conversion.hpp"
#include "parse.hpp"
#include "log.hpp"
#include "error.hpp"

DEFINE_ERROR(interface_error);

class interface {
public:
  double data[4];
  int count{0};
  colorspace from{colorspaces::rgb};
  colorspace to{colorspaces::rgb};
  bool clamp, quit, stay;

  std::string add_term(std::string&&);
  std::string get_terms() const;
  void clear();
  
private:
  std::stringstream output_stream;
  std::vector<std::string> waiting_terms;
  
  template<bool execute, bool expanded>
  bool control_term(const std::string& term);
  void feed_waiting_term(const std::string& term, double data);
};

