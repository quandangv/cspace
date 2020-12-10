#include "interface.hpp"

#include <iomanip>

#include "format.hpp"

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

// Template for printint help and processing long/short switches
template<bool help, bool expanded>
bool interface::switches(const string& term) {
  #define INDENT(size) << endl << std::setw(size) << std::left 

  // Start of the help message {{{
  constexpr int term_indent = 24 + FORMAT_SIZE * 3;
  if constexpr(help && !expanded) {
    logger::debug<scope>("Interface: help called");
    cout << "Converts colors from one color space to another.\n" FORMAT_GREEN_BOLD(Usage: cspace [TERM] [TERM] [DATA] ...) "\n  Data are floating-point numbers that will be the input for the conversions\n  Terms are one of the following:\n";
    cout INDENT(term_indent) << "  " FORMAT_GREEN({colorspace}:) FORMAT_EMPTY2 << "Convert from " FORMAT_BLUE({colorspace}) " (RGB by default)";
    cout INDENT(term_indent) << "  " FORMAT_GREEN({colorspace}!) FORMAT_EMPTY2 << "Convert to " FORMAT_BLUE({colorspace}) " (RGB by default)";
  }
  // }}}

  // Body of the help message / Processing the control terms {{{

  // Start of a control term
  #define CONTROL_TERM \
    { \
      auto action = [&] {
        
  // End of a control term
  #define CONTROL_TERM_END(a, a_full, tail, a_desc) \
      }; \
      if constexpr(help) { \
        if constexpr(#a == "") \
          cout INDENT(term_indent) << "  " FORMAT_GREEN(a_full) FORMAT_EMPTY " " FORMAT_BLUE(tail) << a_desc; \
        else \
          cout INDENT(term_indent) << "  " FORMAT_GREEN(a.) ", " FORMAT_GREEN(a_full) " " FORMAT_BLUE(tail) << a_desc; \
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

  // Control terms that sets a bool field of the same name
  #define FLAG_TERM(a, a_full, tail, a_desc) CONTROL_TERM a_full = true; CONTROL_TERM_END(a, a_full!, tail, a_desc)

  // Control terms that add to waiting_terms
  #define WAIT_TERM(a, a_full, tail, a_desc) CONTROL_TERM add_term_eater(#a_full); CONTROL_TERM_END(a, a_full:, tail, a_desc)

  WAIT_TERM(c, clamp, {on/off/!}, "Clamp each output to the range of its colorspace")

  WAIT_TERM(, mod, {comp} {op}{val}, "Apply modifications to the color before converting")
  WAIT_TERM(i, inter, {cspace}, "Colorspace in which to apply modifications")

  CONTROL_TERM
  print_help();
  CONTROL_TERM_END(h, help!, , "Show this help message")

  CONTROL_TERM
  cout << list_colorspaces(" ") << endl;
  CONTROL_TERM_END(, colorspaces?, , "List supported colorspaces")

  CONTROL_TERM
  auto tmp = from;
  from = to;
  to = tmp;
  CONTROL_TERM_END(w, swap!, , "Swap 'from' and 'to' colorspaces")

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

  WAIT_TERM(p, precision, {num}, "Set output precision to " FORMAT_BLUE({num}) " decimal places")
  
  FLAG_TERM(q, quit, , "Quit program")
  
  FLAG_TERM(s, stay, , "Wait for further input rather that quitting immediately")
  #undef CONTROL_TERM
  #undef CONTROL_TERM_END
  #undef FLAG_TERM
  #undef WAIT_TERM
  // }}}

  // Ending of the help message {{{
  if constexpr(help && !expanded) {
    constexpr int example_indent = 39 + FORMAT_SIZE;
    cout << "\n\n  Terms only affect the conversions that take place after it\n  Passing '!' to on/off terms would toggle them\n  Supported colorspaces are:\n    " << list_colorspaces(", ") << endl;
    cout << "\n" FORMAT_GREEN_BOLD(Example commands:) "\n";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace hsv! FF0000h) << "Convert #FF0000 to HSV";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace hsl! 1 0 0) << "Convert #FF0000 to HSL";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace cielab! hsl: 180 0.5 0.5) << "From HSL to CIELab";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace clamp: on rgb! hsl: 180 0 1.1) << "From HSL to RGB and clamp to RGB range";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace p. 9 CIELab! 0AFh) << "#00AAFF to Lab with 9 decimal places";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace p. 9 CIELab! FFFF0000FFFFh) << "Convert 16-bit colors";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace hsv! 80FF0000h) << "#80FF0000 in argb format to HSV";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace HSV! xxxa! FF000080H) << "#FF000080 in rgba format to HSV";
    cout INDENT(example_indent) << "  " FORMAT_GREEN(cspace cs.) << "Activate clamping and wait for input";
    cout << endl;
  }
  // }}}
  return false;
#undef INDENT
}

void interface::print_help() {
  switches<true, false>("");
}

void interface::process_short_switches(const string& names) {
  switches<false, false>(names);
}

bool interface::process_long_switch(const string& name) {
  return switches<false, true>(name);
}
