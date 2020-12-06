#include "interface.hpp"

#include <sstream>
#include <iomanip>

using namespace std;

constexpr const char scope[] = "interface";

template<bool execute, bool expanded>
bool interface::control_term(const string& term) {
  // Start of the help message {{{
  constexpr int term_indent = 20;
  constexpr int example_indent = 36;
#define INDENT(size) << endl << setw(size) << left 
  if constexpr(!execute) {
    cout << "Usage: cspace [TERM] [TERM] ...\n";
    cout << "Converts colors from one color space to another. Terms are floating-point numbers that will be the input for the conversions. They can also be one of the following:\n";
    cout INDENT(term_indent) << "  {colorspace}:" << "Convert from {colorspace} (RGB by default)";
    cout INDENT(term_indent) << "  {colorspace}!" << "Convert to {colorspace} (RGB by default)";
  }
  // }}}

  // Body of the help message / Processing the control terms {{{
#define CONTROL_TERM(a_full) \
  auto action_##a_full = [&] {
#define CONTROL_TERM_END(a, a_full, a_desc) \
  }; \
  if constexpr(!execute) { \
    if constexpr(#a == "") \
      cout INDENT(term_indent) << "  "#a_full"!" << a_desc; \
    else \
      cout INDENT(term_indent) << "  "#a"., "#a_full"!" << a_desc; \
  } else if constexpr(expanded) { \
    if (term == #a_full) { \
      action_##a_full(); \
      return true; \
    } \
  } else if constexpr(#a != "") { \
    if (term.find(#a) != string::npos) \
    action_##a_full(); \
  }
#define FLAG_CONTROL(a, a_full, a_desc) CONTROL_TERM(a_full) a_full = true; CONTROL_TERM_END(a, a_full, a_desc)

  FLAG_CONTROL(c, clamp, "Clamp each output to the range of its colorspace")
  
  CONTROL_TERM(help)
  control_term<false, false>("");
  CONTROL_TERM_END(h, help, "Show this help message")
  
  CONTROL_TERM(precision)
  waiting_terms.emplace_back("precision");
  CONTROL_TERM_END(p, precision, "Set output precision")
  
  FLAG_CONTROL(q, quit, "Quit program")
  
  FLAG_CONTROL(s, stay, "Wait for further input rather that quitting immediately")
#undef CONTROL_TERM
#undef CONTROL_TERM_END
  // }}}

  // Ending of the help message {{{
  if constexpr(!execute) {
    cout << "\n\nTerms affecting the output (such as clamp, precision) must appear before the input to take effect\n";
    cout << "\nExample commands:";
    cout INDENT(example_indent) << "  cspace hsv! #FF0000" << "Convert #FF0000 to HSV";
    cout INDENT(example_indent) << "  cspace hsl! 1 0 0" << "Convert #FF0000 to HSL";
    cout INDENT(example_indent) << "  cspace cielab! hsl: 180 0.5 0.5" << "Convert from HSL to CIELab";
    cout INDENT(example_indent) << "  cspace c. rgb! hsl: 180 0.5 1.1" << "Convert from HSL to RGB and clamp to RGB range";
    cout << endl;
  }
#undef INDENT
  // }}}
  return !execute || !expanded;
}

std::string interface::add_term(string&& term) {
  log::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  if (int len; sscanf(term.c_str(), "%lf%n", &data[count], &len) == 1 && len == term.size()) {
    
    // Successfully parsed the term as a number, check for waiting terms to feed
    if (!waiting_terms.empty()) {
      // This will not increment count, which means the parsed term is not considered data
      feed_waiting_term(waiting_terms.back(), data[count]);
      waiting_terms.pop_back();
    } else if (++count == colorspaces::component_count(from)) {
      // Use the parsed term as data for color conversion
      log::debug<scope>("Before conversion: " + to_string(&data[0], count) + ", from:" + to_string(from) + ", to: " + to_string(to));
      count = 0;
      colorspaces::convert(&data[0], from, to);
      if (clamp) colorspaces::clamp(&data[0], to);
      // print data to output stream
      output_stream.clear();
      output_stream << data[0];
      for(int i = 1, count = colorspaces::component_count(to); i < count; i++)
        output_stream << ' ' << data[i];
      return output_stream.str();
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

void interface::feed_waiting_term(const string& term, double data) {
  if (term == "precision")
    output_stream << std::setprecision((int)data);
  else throw interface_error("Interface: Unknown waiting term: " + term);
}
