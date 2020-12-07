#include "interface.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>

using std::cout;
using std::string;
using std::endl;

constexpr const char scope[] = "interface";

template<bool execute, bool expanded>
bool interface::control_term(const string& term) {
  if constexpr(!execute)
    logger::debug<scope>("Interface: help called");
  // Start of the help message {{{
  constexpr int term_indent = 20;
  constexpr int example_indent = 36;
#define INDENT(size) << endl << std::setw(size) << std::left 
  if constexpr(!execute) {
    cout << "Converts colors from one color space to another.\nUsage: cspace [TERM] [TERM] ...\n";
    cout << "Terms are floating-point numbers that will be the input for the conversions. They can also be one of the following:\n";
    cout INDENT(term_indent) << "  {colorspace}:" << "Convert from {colorspace} (RGB by default)";
    cout INDENT(term_indent) << "  {colorspace}!" << "Convert to {colorspace} (RGB by default)";
  }
  // }}}

  // Body of the help message / Processing the control terms {{{
#define CONTROL_TERM \
  { \
    auto action = [&] {
#define CONTROL_TERM_END(a, a_full, a_desc) \
    }; \
    if constexpr(!execute) { \
      if constexpr(#a == "") \
        cout INDENT(term_indent) << "  "#a_full << a_desc; \
      else \
        cout INDENT(term_indent) << "  "#a"., "#a_full << a_desc; \
    } else if constexpr(expanded) { \
      if (term == #a_full) { \
        action(); \
        return true; \
      } \
    } else if constexpr(#a != "") { \
      if (term.find(#a) != string::npos) \
      action(); \
    } \
  }
#define FLAG_CONTROL(a, a_full, a_desc) CONTROL_TERM a_full = true; CONTROL_TERM_END(a, a_full!, a_desc)

  FLAG_CONTROL(c, clamp, "Clamp each output to the range of its colorspace")

  CONTROL_TERM
  logger::debug<scope>("Interface: help switch called");
  control_term<false, false>("");
  CONTROL_TERM_END(h, help!, "Show this help message")
  
  CONTROL_TERM
  take_alpha = true;
  CONTROL_TERM_END(a, alpha:, "Read alpha along with other components")
  
  CONTROL_TERM
  alpha_first = true;
  CONTROL_TERM_END(, axxx!, "Read and write alpha component before other components")
  
  CONTROL_TERM
  alpha_first = false;
  CONTROL_TERM_END(, xxxa!, "Read and write alpha component after other components")
  
  CONTROL_TERM
  hex = true;
  output_stream << std::uppercase << std::setfill('0') << std::hex;
  to = colorspaces::rgb;
  CONTROL_TERM_END(, hex!, "Print output colors in hexedecimal code")

  CONTROL_TERM
  waiting_terms.emplace_back("precision");
  CONTROL_TERM_END(p, precision!, "Set output precision")
  
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
  logger::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  if (int len; sscanf(term.c_str(), "%lf%n", &data[count], &len) == 1 && len == term.size()) {
    
    // Successfully parsed the term as a number, check for waiting terms to feed
    if (!waiting_terms.empty()) {
      // This will not increment count, which means the parsed term is not considered data
      feed_waiting_term(waiting_terms.back(), data[count]);
      waiting_terms.pop_back();
    } else if (++count == colorspaces::component_count(from) + (int)take_alpha) {
      if (take_alpha) {
        if (alpha_first) {
          alpha = data[0];
          for(int i = 1; i < count; i++)
            data[i-1] = data[i];
        } else alpha = data[count-1];
        count--;
      }
      return pop_data(from, to);
    }
  } else {
    
    // The term isn't data, check if it is a valid control term
    if (term == "--help") {
      control_term<false, false>("");
    } else if (!control_term<true, true>(term)) {
      auto control_char = term.back();
      term.pop_back();
      switch(control_char) {
      case ':':
        from = stospace(move(term));
        break;
      case '!':
        if (hex) {
          hex = false;
          output_stream << std::dec << std::setfill(' ');
        }
        to = stospace(move(term));
        break;
      case '.':
        logger::debug<scope>("Interface: single-character switches");
        control_term<true, false>(term);
        break;
      case 'h':
      case 'H':
        {
          unsigned int a, r, g, b;
          auto divider = parse_code(term, a, r, g, b, alpha_first);
          if (divider != 0) {
            makesure_empty();
            alpha = static_cast<double>(a) / divider;
            data[0] = static_cast<double>(r) / divider;
            data[1] = static_cast<double>(g) / divider;
            data[2] = static_cast<double>(b) / divider;
            return pop_data(colorspaces::rgb, to);
          }     
        }
        // fall through
      default:
        term.push_back(control_char);
        logger::error("Parsing: Unknown term '" + term + "'");
        break;
      }
    }
  }
  return "";
}

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

void interface::feed_waiting_term(const string& term, double data) {
  if (term == "precision")
    output_stream << std::setprecision((int)data);
  else throw interface_error("Interface: Unknown waiting term: " + term);
}

string interface::pop_data(colorspace from, colorspace to) {
  this->count = 0;
  // Use the parsed term as data for color conversion
  colorspaces::convert(&data[0], from, to);
  if (clamp || hex) colorspaces::clamp(&data[0], to);
  
  // print data to output stream
  output_stream.str("");
  if (hex) {

    // print hex code
    // std::hex have already been passed to output_stream in the hex! term
    if (alpha != 1.0 && alpha_first) {
      output_stream << std::setw(2) << (int)round(alpha * 255.0);
      alpha = 1.0;
    }
    for(int i = 0, count = colorspaces::component_count(to); i < count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
    if (alpha != 1.0 && !alpha_first) {
      output_stream << std::setw(2) << (int)round(alpha * 255.0);
      alpha = 1.0;
    }
  } else {
    if (alpha != 1.0 & alpha_first) {
      output_stream << alpha << separator;
      alpha = 1.0;
    }
    output_stream << data[0];
    for(int i = 1, count = colorspaces::component_count(to); i < count; i++)
      output_stream << separator << data[i];
    if (alpha != 1.0 & !alpha_first) {
      output_stream << separator << alpha;
      alpha = 1.0;
    }
  }
  return output_stream.str();
}

void interface::makesure_empty() const {
  if (count) {
    logger::warn("Interface: There is unprocessed data: " + to_string(&data[0], count) + ". They will be discarded");
  }
}
