#include "interface.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>

#include "token_iterator.hpp"
#include "setting.hpp"

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

interface::interface() {
  use_hex(true);
}

// Apply the right operator to the right component, with the right value
void mod::apply(double* data) const {
  auto& target = data[component];
  switch(op) {
  case '+': target += value; break;
  case '-': target -= value; break;
  case '*': target *= value; break;
  case '/': target /= value; break;
  case '=': target = value; break;
  default: throw interface_error("Mod: Unknown operator: " + string{op});
  }
}

mod::mod(token_iterator& it, colorspace space) {
  logger::debug<scope>("Construct mod: " + it.input);
  // The argument for this setting follows the format: <component> <operator> <value>, ...
  // First token is the component
  if (!it.next_token_base<std::isalnum>()) {
    component = 0;
    op = 0;
    value = 0;
    return;
  }
  component = colorspaces::parse_component(it.token().data(), space);
  logger::debug<scope>("Mod component: '" + it.token() + "', value: " + to_string(component));

  // Next is the operator, which takes a single character
  if (!it.next_token_base<std::ispunct>() || it.token().empty())
    logger::warn("Interface-mod: Missing component operator and value in: " + it.input);
  op = it.token()[0];
  logger::debug<scope>("Mod operator: " + it.token());

  // Last is the value
  if (!it.next_token() || it.token().empty())
    logger::warn("Interface-mod: Missing value in: " + it.input);
  auto token = it.token();
  if (token.back() == ',') {
    token.pop_back();
  }
  if (!parse(token.data(), value))
    throw interface_error("mod: Can't parse numerical value: " + it.token());
}

// Returns the state of the interface
string interface::get_state() {
  std::stringstream output;
  output << to_string(from) << ": "
         << to_string(to) << "! ";
  output << to_string(&data[0], data_count);
  return output.str();
}

// Clears the state of the interface
void interface::clear() {
  from = colorspaces::rgb;
  to = colorspaces::rgb;
  data_count = 0;
  alpha = false;
}

// Process an argument as a waiting term
void interface::feed_term_eater(string&& arg) {
  eater->eat(move(arg), *this);
  eater = nullptr;
}

// Turns on or off hexedecimal output mode
bool interface::use_hex(bool value) {
  if (value) {
    output_stream << std::uppercase << std::setfill('0') << std::hex;
    to = colorspaces::rgb;
  } else {
    output_stream << std::dec << std::setfill(' ');
  }
  return value;
}

bool interface::use_hex() {
  return output_stream.flags() & std::ios::hex;
}

// Called before discarding the data
// Display a warning message if there is unprocessed data
void interface::makesure_empty() {
  if (data_count) {
    logger::warn("Interface: There is unprocessed data: " + to_string(&data[0], data_count) +". It will be discarded");
  }
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
  #define INDENT(size) << endl << std::setw(size) << std::left 
  cout << "Converts colors from one color space to another.\nUsage: cspace [TERM] [TERM] [DATA] ...\n  Data are floating-point numbers that will be the input for the conversions\n  Terms are one of the following:\n";
  cout INDENT(term_indent) << "  {colorspace}:" << "Convert from {colorspace}) (RGB by default)";
  cout INDENT(term_indent) << "  {colorspace}!" << "Convert to {colorspace} (RGB by default)";
  for(auto& s : all_settings) {
    s->print_help();
  }
  cout << "\n\n  Terms only affect the conversions that take place after it\n  Passing '!' to on/off terms would toggle them\n  Supported colorspaces are:\n    " << list_colorspaces(", ") << endl;
  cout << "\nExample commands:\n";
  cout INDENT(example_indent) << "  cspace hsv! FF0000h" << "Convert #FF0000 to HSV";
  cout INDENT(example_indent) << "  cspace hsl! 1 0 0" << "Convert #FF0000 to HSL";
  cout INDENT(example_indent) << "  cspace hsl! 1, 0, 0" << "Comma-separated RGB to HSL";
  cout INDENT(example_indent) << "  cspace cielab! hsl: 180 0.5 0.5" << "From HSL to CIELab";
  cout INDENT(example_indent) << "  cspace p. 9 CIELab! 0AFh" << "#00AAFF to Lab with 9 decimal places";
  cout INDENT(example_indent) << "  cspace p. 9 CIELab! FFFF0000FFFFh" << "Convert 16-bit colors";
  cout INDENT(example_indent) << "  cspace hsv! 80FF0000h" << "#80FF0000 in ARGB format to HSV";
  cout INDENT(example_indent) << "  cspace hsv! 0.5, 1, 0, 0" << "Comma-separated ARGB color to HSV";
  cout INDENT(example_indent) << "  cspace HSV! xxxa! FF000080H" << "#FF000080 in RGBA format to HSV";
  cout INDENT(example_indent) << "  cspace ps. 9" << "Set percision to 9 and wait for input";
  cout INDENT(example_indent) << "  cspace mod: J *1.5 hex: on FF0022h" << "Multiply lightness of #FF0022 by 1.5";
  cout INDENT(example_indent) << "" << "J is the component for lightness in Jzazbz";
  cout << endl;
  #undef INDENT
}

// Short switches are indivitual characters, each representing a different settings, in a single term
void interface::process_short_switches(const string& names) {
  for(auto& s : all_settings) {
    s->on_short_switches(names, *this);
  }
}

// Long switches are terms that represent a specific setting
bool interface::process_long_switch(const string& name) {
  for(auto& s : all_settings) {
    if (s->on_long_switch(name, *this))
      return true;
  }
  return false;
}
