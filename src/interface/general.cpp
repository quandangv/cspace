#include "interface.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>

#include "token_iterator.hpp"

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

interface::interface() {
  use_hex(true);
}

void mod::apply(double* data) const {
  auto& target = data[component];
  switch(op) {
  case '+': target += value; break;
  case '-': target -= value; break;
  case '*': target *= value; break;
  case '/': target /= value; break;
  case '=': target = value; break;
  default: throw interface_error("Unknown operator: " + string{op});
  }
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
  auto c_str = arg.c_str();
  if (term_eater == "precision") {
    term_eater.clear();
    if (int val; parse(c_str, val)) {
      output_stream << std::setprecision(val);
    } else 
      throw interface_error("precision: Unknown term argument: "+arg);
  } else if (term_eater == "inter") {
    term_eater.clear();
    inter = stospace(arg);
  } else if (term_eater == "mod") {
    logger::debug<scope>("Arguemnt: " + arg);
    if (strcasecmp(c_str, "none") == 0) {
      modifications.clear();
      term_eater.clear();
    } else {
      token_iterator it(move(arg));
      while (it.next_token_base<std::isalnum>()) {
        auto comp = colorspaces::parse_component(it.token().data(), inter);
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
              modifications.emplace_back(comp, op, value);
            } else throw interface_error("mod: Can't parse numerical value: " + it.token());
          } else logger::warn("Interface-mod: Missing value in: " + string(c_str));
        } else logger::warn("Interface-mod: Missing component operator and value in: " + string(c_str));
      }
      term_eater.clear();
    }
  } else if (term_eater == "hex") {
    term_eater.clear();
    if (bool val; parse(c_str, val)) {
      use_hex(val);
    } else if (arg == "!")
      use_hex(!use_hex());
    else
      throw interface_error("hex: Unknown term argument: "+arg);
  } else throw application_error("Interface: Unknown term eater: " + term_eater);
}

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

void interface::add_term_eater(string&& name) {
  if (!term_eater.empty())
    logger::warn("Term dropped without taking its required argument: " + term_eater);
  term_eater = forward<string>(name);
}
