#pragma once

#include <string>

#include "interface.hpp"

using std::string;

class interface;

// Settings are options of the interface that can be changed through user input
// An instance of this class determines how to parse the user input and what action to take
struct setting {
  virtual void on_short_switches(const string&, interface&) const {}
  virtual bool on_long_switch(const string&, interface&) const { return false; }
  virtual void print_help() const {}
};

// This is the base class for settings that support long switch but not short switch
struct basic_setting : setting {
  virtual string long_name() const = 0;
  virtual string description() const = 0;
  virtual string arguments() const { return ""; }
  virtual void action(interface&) const = 0;
  bool on_long_switch(const string&, interface&) const;
  void print_help() const;
};

// Base class for settings that support long and short switch
struct short_setting : basic_setting {
  virtual char short_name() const = 0;
  void on_short_switches(const string&, interface&) const;
  void print_help() const;
};

// Objects that would take a term as argument from the interface
struct term_eater {
  virtual void eat(string&&, interface&) const = 0;
  virtual ~term_eater() {}
};

// Base class for settings that would register itself as a term_eater
template<typename T>
struct eater_setting : T, term_eater {
  void action(interface& intf) const {
    intf.add_term_eater(this);
  }
};

// Contains instances of all the settings used by the interface
extern const std::vector<setting*> all_settings;

