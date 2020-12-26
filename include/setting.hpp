#pragma once

#include <string>

#include "interface.hpp"

using std::string;

class interface;

struct setting {
  virtual void on_short_switches(const string&, interface&) const {}
  virtual bool on_long_switch(const string&, interface&) const { return false; }
  virtual void print_help() const {}
};

struct basic_setting : setting {
  virtual string long_name() const = 0;
  virtual string description() const = 0;
  virtual string arguments() const { return ""; }
  virtual void action(interface&) const = 0;
  bool on_long_switch(const string&, interface&) const;
  void print_help() const;
};

struct short_setting : basic_setting {
  virtual char short_name() const = 0;
  void on_short_switches(const string&, interface&) const;
  void print_help() const;
};

struct term_eater {
  virtual void eat(string&&, interface&) const = 0;
  virtual ~term_eater() {}
};

template<typename T>
struct eater_setting : T, term_eater {
  void action(interface& intf) const {
    intf.add_term_eater(this);
  }
};

extern const std::vector<setting*> all_settings;

