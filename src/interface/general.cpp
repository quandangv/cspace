#include "interface.hpp"

#include <sstream>
#include <iomanip>

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

// Returns the state of the interface
string interface::get_state() const {
  std::stringstream output;
  output << to_string(from) << ": "
         << to_string(to) << "!";
  for(int i = 0; i < count; i++)
    output << ' ' << data[i];
  return output.str();
}

// Clears the state of the interface
void interface::clear() {
  from = colorspaces::rgb;
  to = colorspaces::rgb;
  count = 0;
}

// Process an argument as a waiting term
void interface::feed_waiting_term(const string& term, string&& arg) {
  if (term == "precision") {
    if (int val; parse(arg, val))
      output_stream << std::setprecision(val);
    else throw interface_error("Interface-precision: Unknown term argument: "+arg);
    
  // Wait terms that expects a boolean argument
  #define BOOL_WAIT_TERM(name) \
  } else if (term == #name) { \
    if (bool val; parse(arg, val)) { \
      name = val; \
    } else if (arg == "!") \
      name = !name; \
    else throw interface_error("Interface-"#name": Unknown term argument: "+arg);
  BOOL_WAIT_TERM(alpha)
  BOOL_WAIT_TERM(clamp)
  } else throw application_error("Interface: Unknown waiting term: " + term);
#undef BOOL_WAIT_TERM
}

// Called before discarding the data
// Display a warning message if there is unprocessed data
void interface::makesure_empty() const {
  if (count) {
    logger::warn("Interface: There is unprocessed data: " + to_string(&data[0], count) + ". They will be discarded");
  }
}
