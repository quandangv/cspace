#include "interface.hpp"
#include "logger.hpp"
#include "token_iterator.hpp"
#include "setting.hpp"
#include "parse.hpp"

#include <iomanip>
#include <iostream>

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
