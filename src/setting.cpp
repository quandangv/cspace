#include "setting.hpp"

#include <iostream>
#include <iomanip>

#include "interface.hpp"
#include "logger.hpp"

constexpr int term_indent = 24;
constexpr char scope[] = "setting";

bool basic_setting::on_long_switch(const string& name, interface& intf) const {
  if (name == long_name()) {
    logger::debug<scope>("Name: " + name + ", Term name: " + long_name());
    action(intf);
    return true;
  }
  return false;
}

void basic_setting::print_help() const {
  std::cout << std::endl << std::setw(term_indent) << std::left << "  " + long_name() + " " + arguments() << description();
}

void short_setting::on_short_switches(const string& switches, interface& intf) const {
  if (switches.find(short_name()) != string::npos) {
    action(intf);
  }
}

void short_setting::print_help() const {
  std::cout << std::endl << std::setw(term_indent) << std::left << "  " + string{short_name()} + "., " + long_name() + " " + arguments() << description();
}

