#include "interface.hpp"

#include <sstream>

using namespace std;

constexpr const char scope[] = "interface";

std::string interface::add_term(string&& term) {
  log::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  if (int len; sscanf(term.c_str(), "%lf%n", &data[count], &len) == 1 && len == term.size()) {
    
    // Successfully parsed the term as a number, use it as data for conversion
    if (++count == colorspaces::component_count(from)) {
      log::debug<scope>("Before conversion: " + to_string(&data[0], count) + ", from:" + to_string(from) + ", to: " + to_string(to));
      count = 0;
      colorspaces::convert(&data[0], from, to);
      return to_string(&data[0], to);
    }
  } else {
    
    // The term isn't data, check if it is a valid control term
    auto control_char = term.back();
    term.pop_back();
    switch(control_char) {
    case ':':
      from = stospace(move(term));
      break;
    case '!':
      log::debug<scope>("Conversion Target: Raw '" + term + "'");
      to = stospace(move(term));
      log::debug<scope>("Conversion Target: " + to_string(to));
      break;
    default:
      log::error("Parsing: Unknown argument '" + term + "'");
      break;
    }
  }
  return "";
}

string interface::get_terms() {
  stringstream output;
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
