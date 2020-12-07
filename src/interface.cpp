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
  constexpr int term_indent = 25;
  constexpr int example_indent = 40;
#define INDENT(size) << endl << std::setw(size) << std::left 
  if constexpr(!execute) {
    cout << "Converts colors from one color space to another.\nUsage: cspace [TERM] [TERM] ...\n";
    cout << "Terms are floating-point numbers that will be the input for the conversions, but they can also be one of the following:\n";
    cout INDENT(term_indent) << "  {colorspace}:" << "Convert from {colorspace} (RGB by default)";
    cout INDENT(term_indent) << "  {colorspace}!" << "Convert to {colorspace} (RGB by default)";
  }
  // }}}

  // Body of the help message / Processing the control terms {{{
#define CONTROL_TERM \
  { \
    auto action = [&] {
#define CONTROL_TERM_END(a, a_full, tail, a_desc) \
    }; \
    if constexpr(!execute) { \
      if constexpr(#a == "") \
        cout INDENT(term_indent) << "  "#a_full" "#tail << a_desc; \
      else \
        cout INDENT(term_indent) << "  "#a"., "#a_full" "#tail << a_desc; \
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
#define FLAG_TERM(a, a_full, tail, a_desc) CONTROL_TERM a_full = true; CONTROL_TERM_END(a, a_full!, tail, a_desc)
#define WAIT_TERM(a, a_full, tail, a_desc) CONTROL_TERM waiting_terms.emplace_back(#a_full); CONTROL_TERM_END(a, a_full:, tail, a_desc)

  WAIT_TERM(c, clamp, {on/off}, "Clamp each output to the range of its colorspace")

  CONTROL_TERM
  logger::debug<scope>("Interface: help switch called");
  control_term<false, false>("");
  CONTROL_TERM_END(h, help!, , "Show this help message")

  WAIT_TERM(a, alpha, {on/off}, "Read alpha along with other components")
  
  CONTROL_TERM
  alpha_first = true;
  CONTROL_TERM_END(, axxx!, , "Read and write alpha component before other components")
  
  CONTROL_TERM
  alpha_first = false;
  CONTROL_TERM_END(, xxxa!, , "Read and write alpha component after other components")
  
  CONTROL_TERM
  hex = true;
  output_stream << std::uppercase << std::setfill('0') << std::hex;
  to = colorspaces::rgb;
  CONTROL_TERM_END(, hex!, , "Print output colors in hexedecimal code")

  WAIT_TERM(p, precision, {num}, "Set output precision to {num} decimal places")
  
  FLAG_TERM(q, quit, , "Quit program")
  
  FLAG_TERM(s, stay, , "Wait for further input rather that quitting immediately")
#undef CONTROL_TERM
#undef CONTROL_TERM_END
#undef FLAG_TERM
#undef WAIT_TERM
  // }}}

  // Ending of the help message {{{
  if constexpr(!execute) {
    cout << "\n\nTerms affecting the output (such as clamp, precision) must appear before the input to take effect";
    cout << "\nSupported colorspaces: RGB, HSV, HSL, XYZ, CIELab, CIELCh, Jzazbz, JzCzhz\n";
    cout << "\nExample commands:";
    cout INDENT(example_indent) << "  cspace hsv! FF0000h" << "Convert #FF0000 to HSV";
    cout INDENT(example_indent) << "  cspace hsl! 1 0 0" << "Convert #FF0000 to HSL";
    cout INDENT(example_indent) << "  cspace cielab! hsl: 180 0.5 0.5" << "From HSL to CIELab";
    cout INDENT(example_indent) << "  cspace clamp: on rgb! hsl: 180 0 1.1" << "From HSL to RGB and clamp to RGB range";
    cout INDENT(example_indent) << "  cspace p. 9 CIELab! 0AFh" << "#00AAFF to Lab with 9 decimal places";
    cout INDENT(example_indent) << "  cspace hsv! 80FF0000h" << "#80FF0000 in argb format to HSV";
    cout INDENT(example_indent) << "  cspace HSV! xxxa! FF000080H" << "#FF000080 in rgba format to HSV";
    cout INDENT(example_indent) << "  cspace cs." << "Activate clamping and wait for input";
    cout << endl;
  }
#undef INDENT
  // }}}
  return !execute || !expanded;
}

std::string interface::add_term(string&& term) {
  logger::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  if (!waiting_terms.empty()) {
    logger::debug<scope>("Interface: Feed waiting terms: "+term);
    feed_waiting_term(waiting_terms.back(), move(term));
    waiting_terms.pop_back();
  } else if (parse(term, data[count])) {
    if (++count == colorspaces::component_count(from) + (int)alpha) {
      if (alpha) {
        if (alpha_first) {
          alpha_val = data[0];
          for(int i = 1; i < count; i++)
            data[i-1] = data[i];
        } else alpha_val = data[count-1];
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
            alpha_val = static_cast<double>(a) / divider;
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
    if (alpha_val != 1.0 && alpha_first) {
      output_stream << std::setw(2) << (int)round(alpha_val * 255.0);
      alpha_val = 1.0;
    }
    for(int i = 0, count = colorspaces::component_count(to); i < count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
    if (alpha_val != 1.0 && !alpha_first) {
      output_stream << std::setw(2) << (int)round(alpha_val * 255.0);
      alpha_val = 1.0;
    }
  } else {
    if (alpha_val != 1.0 & alpha_first) {
      output_stream << alpha_val << separator;
      alpha_val = 1.0;
    }
    output_stream << data[0];
    for(int i = 1, count = colorspaces::component_count(to); i < count; i++)
      output_stream << separator << data[i];
    if (alpha_val != 1.0 & !alpha_first) {
      output_stream << separator << alpha_val;
      alpha_val = 1.0;
    }
  }
  return output_stream.str();
}

void interface::makesure_empty() const {
  if (count) {
    logger::warn("Interface: There is unprocessed data: " + to_string(&data[0], count) + ". They will be discarded");
  }
}
