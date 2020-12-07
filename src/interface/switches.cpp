#include "interface.hpp"

#include <iomanip>

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

template<bool help, bool _short>
bool interface::switches(const string& term) {
  if constexpr(help)
    logger::debug<scope>("Interface: help called");
  // Start of the help message {{{
  constexpr int term_indent = 25;
  constexpr int example_indent = 40;
#define INDENT(size) << endl << std::setw(size) << std::left 
  if constexpr(help) {
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
    if constexpr(help) { \
      if constexpr(#a == "") \
        cout INDENT(term_indent) << "  "#a_full" "#tail << a_desc; \
      else \
        cout INDENT(term_indent) << "  "#a"., "#a_full" "#tail << a_desc; \
    } else if constexpr(!_short) { \
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
  print_help();
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
  if constexpr(help) {
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
  return help || _short;
}

void interface::print_help() {
  switches<true, false>("");
}

void interface::process_short_switches(const string& names) {
  switches<false, true>(names);
}

bool interface::process_long_switch(const string& name) {
  return switches<false, false>(name);
}
