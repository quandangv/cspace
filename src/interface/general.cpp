#include "interface.hpp"
#include "logger.hpp"

#include <iomanip>
#include <iostream>

#include "token_iterator.hpp"
#include "setting.hpp"
#include "parse.hpp"

GLOBAL_NAMESPACE

constexpr char scope[] = "interface";

// Process an argument as a waiting term
void interface::feed_term_eater(string&& arg) {
  eater->eat(move(arg), *this);
  eater = nullptr;
}

// Called before discarding the data
// Display a warning message if there is unprocessed data
void interface::makesure_empty() {
  if (data_count)
    logger::warn("Interface: There is unprocessed data: " + print(&data[0], data_count) +". It will be discarded");
}

void interface::unexpected_comma(const string& term) {
  if (comma) {
    logger::warn("Parsing term: Unexpected comma at: " + term);
    comma = false;
  }
}

// Term eater are objects that consumes input terms to modify the interface
void interface::add_term_eater(const term_eater* e) {
  if (eater != nullptr)
    logger::warn("Term dropped without taking its required argument");
  eater = e;
}

void interface::print_help() {
  constexpr int term_indent = 24, example_indent = 39;
  #define MY_COUT(size) std::cout << std::endl << std::setw(size) << std::left 
  std::cout << "Converts colors from one color space to another.\nUsage: cspace [TERM] [TERM] [DATA] ...\n  Data are floating-point numbers that will be the input for the conversions\n  Terms are one of the following:\n";
  MY_COUT(term_indent) << "  {colorspace}:" << "Convert from {colorspace}) (RGB by default)";
  MY_COUT(term_indent) << "  {colorspace}!" << "Convert to {colorspace} (RGB by default)";
  for(auto& s : all_settings) {
    s->print_help();
  }
  std::cout << "\n\n  Terms only affect the conversions that take place after it\n  Passing '!' to on/off terms would toggle them\n  Supported colorspaces are:\n    " << list_colorspaces(", ") << std::endl;
  std::cout << "\nExample commands:\n";
  MY_COUT(example_indent) << "  cspace hsv! FF0000h" << "Convert #FF0000 to HSV";
  MY_COUT(example_indent) << "  cspace hsl! 1 0 0" << "Convert #FF0000 to HSL";
  MY_COUT(example_indent) << "  cspace hsl! 1, 0, 0" << "Comma-separated RGB to HSL";
  MY_COUT(example_indent) << "  cspace cielab! hsl: 180 0.5 0.5" << "From HSL to CIELab";
  MY_COUT(example_indent) << "  cspace p. 9 CIELab! 0AFh" << "#00AAFF to Lab with 9 decimal places";
  MY_COUT(example_indent) << "  cspace p. 9 CIELab! FFFF0000FFFFh" << "Convert 16-bit colors";
  MY_COUT(example_indent) << "  cspace hsv! 80FF0000h" << "#80FF0000 in ARGB format to HSV";
  MY_COUT(example_indent) << "  cspace hsv! 0.5, 1, 0, 0" << "Comma-separated ARGB color to HSV";
  MY_COUT(example_indent) << "  cspace HSV! xxxa! FF000080H" << "#FF000080 in RGBA format to HSV";
  MY_COUT(example_indent) << "  cspace ps. 9" << "Set percision to 9 and wait for input";
  MY_COUT(example_indent) << "  cspace mod: J *1.5 hex: on FF0022h" << "Multiply lightness of #FF0022 by 1.5";
  MY_COUT(example_indent) << "" << "J is the component for lightness in Jzazbz";
  std::cout << std::endl;
  #undef MY_COUT
}

// Short switches are indivitual characters, each representing a different settings, in a single term
void interface::process_short_switches(const string& names) {
  for(auto& s : all_settings)
    s->on_short_switches(names, *this);
}

// Long switches are terms that represent a specific setting
bool interface::process_long_switch(const string& name) {
  for(auto& s : all_settings)
    if (s->on_long_switch(name, *this))
      return true;
  return false;
}

// Start conversion and return the resulting string
string interface::pop_data(colorspace from) {
  auto result = operate(&data[0], alpha, from);
  data_count = 0;
  alpha = false;
  return result;
}

string interface::pop_data() {
  return pop_data(from);
}

GLOBAL_NAMESPACE_END
