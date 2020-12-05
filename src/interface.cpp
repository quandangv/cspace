#include "interface.hpp"

constexpr const char scope[] = "interface";

std::string interface::take_term(string&& term) {
  log::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  int len;
  if (sscanf(term.c_str(), "%lf%n", &data[count], &len) == 1 && len == term.size()) {
    if (++count == colorspaces::component_count(from)) {
      log::debug<scope>("Before conversion: " + to_string(&data[0], count) + ", from:" + to_string(from) + ", to: " + to_string(to));
      count = 0;
      colorspaces::convert(&data[0], from, to);
      return to_string(&data[0], to);
    }
  } else {
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
