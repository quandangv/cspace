#include "interface.hpp"

#include <sstream>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

string interface::get_terms() const {
  std::stringstream output;
  output << to_string(from) << ": "
         << to_string(to) << "!";
  for(int i = 0; i < count; i++)
    output << ' ' << data[i];
  return output.str();
}

void interface::clear() {
  from = colorspaces::rgb;
  to = colorspaces::rgb;
  count = 0;
}

void interface::feed_waiting_term(const string& name, string&& data) {
  if (name == "precision") {
    if (int val; parse(data, val))
      output_stream << std::setprecision(val);
    else throw interface_error("Interface-precision: Unknown term argument: "+data);
#define BOOL_WAIT_TERM(term) \
  } else if (name == #term) { \
    if (bool val; parse(data, val)) { \
      term = val; \
    } else if (data == "!") \
      term = !term; \
    else throw interface_error("Interface-"#term": Unknown term argument: "+data);
  BOOL_WAIT_TERM(alpha)
  BOOL_WAIT_TERM(clamp)
  } else throw application_error("Interface: Unknown waiting term: " + name);
#undef BOOL_WAIT_TERM
}

void interface::makesure_empty() const {
  if (count) {
    logger::warn("Interface: There is unprocessed data: " + to_string(&data[0], count) + ". They will be discarded");
  }
}
