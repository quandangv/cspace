#include <iostream>

#include "conversion.hpp"
#include "parse.hpp"
#include "log.hpp"
#include "error.hpp"

DEFINE_ERROR(interface_error);

class interface {
  double data[4];
  int count{0};
  colorspace from{colorspaces::rgb};
  colorspace to{colorspaces::rgb};

public:
  std::string add_term(std::string&&);
  std::string get_terms();
  void clear();
};

