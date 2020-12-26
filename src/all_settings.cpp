#include "interface.hpp"

#include <iomanip>

#include "format.hpp"
#include "setting.hpp"
#include "token_iterator.hpp"

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

struct : short_setting {
  string long_name() const { return "help!"; }
  char short_name() const { return 'h'; }
  string description() const { return "Show this help message"; }
  void action(interface& intf) const { intf.print_help(); }
} _help;

struct : basic_setting {
  string long_name() const { return "colorspaces?"; }
  string description() const { return "List supported color spaces"; }
  void action(interface& intf) const { cout << list_colorspaces(" "); }
} _colorspaces;

struct : basic_setting {
  string long_name() const { return "swap!"; }
  string description() const { return "Swap 'from' and 'to' color spaces"; }
  void action(interface& intf) const {
    auto tmp = intf.from;
    intf.from = intf.to;
    intf.to = tmp;
  }
} _swap;

struct : basic_setting {
  string long_name() const { return "axxx!"; }
  string description() const { return "Read and write alpha component before other components"; }
  void action(interface& intf) const { intf.alpha_first = true; }
} _alpha_first;

struct : basic_setting {
  string long_name() const { return "xxxa!"; }
  string description() const { return "Read and write alpha component after other components"; }
  void action(interface& intf) const { intf.alpha_first = false; }
} _alpha_last;

struct : eater_setting<short_setting> {
  string long_name() const { return "precision:"; }
  char short_name() const { return 'p'; }
  string arguments() const { return "{num}"; }
  string description() const { return "Set output precision to {num} decimal places"; }
  void eat(string&& s, interface& intf) const {
    if (int val; parse(s.data(), val)) {
      intf.output_stream << std::setprecision(val);
    } else 
      throw interface_error("precision: Unknown term argument: " + s);
  }
} _precision;

struct : eater_setting<short_setting> {
  string long_name() const { return "inter:"; }
  char short_name() const { return 'i'; }
  string arguments() const { return "{cspace}"; }
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
    if (bool val; parse(s.data(), val)) {
      intf.use_hex(val);
    } else if (s == "!")
      intf.use_hex(!intf.use_hex());
    else
      throw interface_error("hex: Unknown term argument: " + s);
  }
} _hex;

struct : eater_setting<basic_setting> {
  string long_name() const { return "mod:"; }
  string arguments() const { return "{comp}{op}{value}"; }
  string description() const { return "Apply modifications to the color before converting"; }
  void eat(string&& s, interface& intf) const {
    if (strcasecmp(s.data(), "none") == 0) {
      intf.modifications.clear();
    } else {
      token_iterator it(move(s));
      while (it.next_token_base<std::isalnum>()) {
        auto comp = colorspaces::parse_component(it.token().data(), intf.inter);
        if (it.next_token() && !it.token().empty()) {
          auto op = it.token()[0];
          it.return_token(1);
          if (it.next_token() && !it.token().empty()) {
            auto token = it.token();
            if (token.back() == ',') {
              token.pop_back();
            }
            double value;
            if (parse(token.data(), value)) {
              intf.modifications.emplace_back(comp, op, value);
            } else throw interface_error("mod: Can't parse numerical value: " + it.token());
          } else logger::warn("Interface-mod: Missing value in: " + s);
        } else logger::warn("Interface-mod: Missing component operator and value in: " + s);
      }
    }
  }
} _mod;

struct : short_setting {
  string long_name() const { return "stay!"; }
  char short_name() const { return 's'; }
  string description() const { return "Wait for further input rather than quitting immediately"; }
  void action(interface& intf) const { intf.stay = true; }
} _stay;

struct : short_setting {
  string long_name() const { return "quit!"; }
  char short_name() const { return 'q'; }
  string description() const { return "Quit program"; }
  void action(interface& intf) const { intf.quit = true; }
} _quit;

const std::vector<setting*> all_settings = {&_help, &_colorspaces, &_swap, &_alpha_first, &_alpha_last, &_quit, &_stay, &_inter, &_hex, &_precision, &_mod};

