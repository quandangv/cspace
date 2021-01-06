#include "interface.hpp"
#include "logger.hpp"
#include "setting.hpp"
#include "token_iterator.hpp"
#include "parse.hpp"

#include <iomanip>
#include <iostream>

GLOBAL_NAMESPACE

constexpr char scope[] = "setting";

void print_all_help() {
  std::cout << "Converts colors from one color space to another.\nUsage: cspace [TERM] [TERM] [DATA] ...\n  Data are floating-point numbers that will be the input for the conversions\n  Terms are one of the following:\n";
  constexpr int term_indent = 26, example_indent = 39;
  #define MY_COUT(size) std::cout << std::endl << std::setw(size) << std::left 
  MY_COUT(term_indent) << "  {colorspace}:" << "Convert from {colorspace}) (RGB by default)";
  MY_COUT(term_indent) << "  {colorspace}!" << "Convert to {colorspace} (RGB by default)";
  for(auto& s : all_settings) {
    s->print_help();
  }
  std::cout << "\n\n  Terms only affect the conversions that take place after it\n  Passing '!' to on/off terms would toggle them\n  Supported colorspaces are:\n    " << list_colorspaces(", ") << std::endl;
  std::cout << "\nExample commands:\n";
  MY_COUT(example_indent) << "  cspace hsv! '#FF0000'" << "Convert #FF0000 to HSV";
  MY_COUT(example_indent) << "  cspace hsv! FF0000h" << "Another way to use hexedecimal code";
  MY_COUT(example_indent) << "  cspace hsl! 1 0 0" << "Convert #FF0000 to HSL";
  MY_COUT(example_indent) << "  cspace hsl! 1, 0, 0" << "Comma-separated RGB to HSL";
  MY_COUT(example_indent) << "  cspace cielab! hsl: 180 0.5 0.5" << "From HSL to CIELab";
  MY_COUT(example_indent) << "  cspace p. 9 CIELab! 0AFh" << "#00AAFF to Lab with 9 decimal places";
  MY_COUT(example_indent) << "  cspace p. 9 CIELab! FFFF0000FFFFh" << "Convert 16-bit colors";
  MY_COUT(example_indent) << "  cspace hsv! 80FF0000h" << "#80FF0000 in ARGB format to HSV";
  MY_COUT(example_indent) << "  cspace hsv! 0.5, 1, 0, 0" << "Comma-separated ARGB color to HSV";
  MY_COUT(example_indent) << "  cspace HSV! xxxa! FF000080h" << "#FF000080 in RGBA format to HSV";
  MY_COUT(example_indent) << "  cspace ps. 9" << "Set percision to 9 and wait for input";
  MY_COUT(example_indent) << "  cspace mod: J *1.5 hex: on FF0022h" << "Multiply lightness of #FF0022 by 1.5";
  MY_COUT(example_indent) << "" << "J is the component for lightness in Jzazbz";
  std::cout << std::endl;
  #undef MY_COUT
}

struct : setting {
  bool on_long_switch(const string& s, interface&) const {
    if (s == "help" || s == "--help" || s == "-h")
      print_all_help();
    else return false;
    return true;
  }
} _help;

struct : basic_setting {
  string long_name() const { return "colorspaces?"; }
  string description() const { return "List supported color spaces"; }
  void action(interface& intf) const { std::cout << list_colorspaces(" "); }
} _colorspaces;

struct : basic_setting {
  string long_name() const { return "swap"; }
  string description() const { return "Swap 'from' and 'to' color spaces"; }
  void action(interface& intf) const {
    auto tmp = intf.from;
    intf.from = intf.target();
    intf.target(tmp);
  }
} _swap;

struct : basic_setting {
  string long_name() const { return "axxx"; }
  string description() const { return "Read and write alpha component before other components"; }
  void action(interface& intf) const { intf.alpha_first = true; }
} _alpha_first;

struct : basic_setting {
  string long_name() const { return "xxxa"; }
  string description() const { return "Read and write alpha component after other components"; }
  void action(interface& intf) const { intf.alpha_first = false; }
} _alpha_last;

struct : eater_setting<short_setting> {
  string long_name() const { return "precision:"; }
  char short_name() const { return 'p'; }
  string arguments() const { return "<num>"; }
  string description() const { return "Set output precision to <num> decimal places"; }
  void eat(string&& s, interface& intf) const {
    if (int val; parse(tstring(s), val)) {
      intf.output_stream << std::setprecision(val);
    } else 
      throw setting::error("precision: Unknown term argument: " + s);
  }
} _precision;

struct : eater_setting<short_setting> {
  string long_name() const { return "inter:"; }
  char short_name() const { return 'i'; }
  string arguments() const { return "<cspace>"; }
  string description() const { return "Colorspace in which to apply mods, default: Jzazbz"; }
  void eat(string&& s, interface& intf) const {
    intf.inter = stospace(s);
  }
} _inter;

struct : eater_setting<basic_setting> {
  string long_name() const { return "hex:"; }
  string arguments() const { return "on/off/!"; }
  string description() const { return "Print output colors int RGB hexedecimal code"; }
  void eat(string&& s, interface& intf) const {
    if (bool val; parse(s, val)) {
      intf.use_hex(val);
    } else if (s == "!")
      intf.use_hex(!intf.use_hex());
    else
      throw setting::error("hex: Unknown term argument: " + s);
  }
} _hex;

struct : eater_setting<basic_setting> {
  string long_name() const { return "mod:"; }
  string arguments() const { return "'<comp><op><val>'"; }
  string description() const { return "Apply modifications to the color before converting"; }
  void eat(string&& s, interface& intf) const { intf.add_modification(move(s)); }
} _mod;

struct : short_setting {
  string long_name() const { return "stay"; }
  char short_name() const { return 's'; }
  string description() const { return "Wait for further input rather than quitting immediately"; }
  void action(interface& intf) const { intf.stay = true; }
} _stay;

struct : short_setting {
  string long_name() const { return "quit"; }
  char short_name() const { return 'q'; }
  string description() const { return "Quit program"; }
  void action(interface& intf) const { intf.quit = true; }
} _quit;

const std::vector<const setting*> all_settings = {&_help, &_colorspaces, &_swap, &_alpha_first, &_alpha_last, &_quit, &_stay, &_inter, &_hex, &_precision, &_mod};

GLOBAL_NAMESPACE_END
