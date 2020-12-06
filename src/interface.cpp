#include "interface.hpp"

#include <sstream>
#include <iomanip>

using namespace std;

constexpr const char scope[] = "interface";

template<bool execute, bool expanded>
bool interface::control_term(const string& term) {
  if constexpr(!execute)
    cout << "Usage: scpace [TERM1] [TERM2]...\n\n";
#define CONTROL_TERM(a_full) \
  auto action_##a_full = [&] {
#define CONTROL_TERM_END(a, a_full, a_desc) \
  }; \
  if constexpr(!execute) { \
    cout << std::setw(16) << std::left; \
    if constexpr(#a == "") \
      cout << "  "#a_full"!" << a_desc"\n"; \
    else \
      cout << "  "#a"., "#a_full"!" << a_desc"\n"; \
  } else if constexpr(expanded) { \
    if (term == #a_full) { \
      action_##a_full(); \
      return true; \
    } \
  } else if constexpr(#a != "") { \
    if (term.find(#a) != string::npos) \
    action_##a_full(); \
  }
#define FLAG_CONTROL(a, a_full, a_desc) \
  CONTROL_TERM(a_full) a_full = true; CONTROL_TERM_END(a, a_full, a_desc)
  FLAG_CONTROL(c, clamp, "Clamp each output to the range of its colorspace")
  FLAG_CONTROL(q, quit, "Quit program")
  FLAG_CONTROL(s, stay, "Quit program")
  CONTROL_TERM(help)
    control_term<false, false>("");
  CONTROL_TERM_END(h, help, "Show this help message")
#undef CONTROL_TERM
#undef CONTROL_TERM_END
  return !execute || !expanded;
}

std::string interface::add_term(string&& term) {
  log::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  if (int len; sscanf(term.c_str(), "%lf%n", &data[count], &len) == 1 && len == term.size()) {
    
    // Successfully parsed the term as a number, use it as data for conversion
    if (++count == colorspaces::component_count(from)) {
      log::debug<scope>("Before conversion: " + to_string(&data[0], count) + ", from:" + to_string(from) + ", to: " + to_string(to));
      count = 0;
      colorspaces::convert(&data[0], from, to);
      if (clamp)
        colorspaces::clamp(&data[0], to);
      return to_string(&data[0], to);
    }
  } else {
    
    // The term isn't data, check if it is a valid control term
    if (term == "--help") {
      control_term<false, false>("");
      return "";
    }
    auto control_char = term.back();
    term.pop_back();
    switch(control_char) {
    case ':':
      from = stospace(move(term));
      break;
    case '!':
      if (!control_term<true, true>(term))
        to = stospace(move(term));
      break;
    case '.':
      control_term<true, false>(term);
      break;
    default:
      log::error("Parsing: Unknown argument '" + term + "'");
      break;
    }
  }
  return "";
}

string interface::get_terms() const {
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
