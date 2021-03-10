#include "interface.hpp"
#include "common.hpp"
#include "logger.hpp"
#include "parse.hpp"
#include "token_iterator.hpp"

#include <iostream>
#include <iomanip>

GLOBAL_NAMESPACE

bool interface::add_data(double value) {
  data[data_count] = value;
  if (++data_count >= component_count(from)) {
    if (comma) {
      // The alpha component is present, add another component before we start the operation
      if (alpha)
        throw new error("Enough components have been received, excess comma");
      alpha = true;
    } else
      // Got enough components, start the operation
      return pop_data(from);
  }
  comma = false;
  return false;
}

// Process the given term
// If the term triggers an operation, the resulting string is returned
bool interface::silent_add_term(string&& term) {
  unexpected_comma(term);
  if (term.empty()) return false;
  if (term.back() == ',') {
    // Extract the comma from the end of the term
    // The comma is used to signify the presence of the alpha component
    term.pop_back();
    if (term.empty()) {
      logger::warn("Parsing term: Orphaned comma ',', put commas at the end of a term without spaces");
      return false;
    } else
      comma = true;
  }

  if (eater != nullptr) {
    // Feed term eater, which are terms that takes in additional arguments
    feed_term_eater(move(term));
    unexpected_comma(term);
  } else if (double value; parse(term, value)) {
    return add_data(value);
  } else {
    // Processing the switches, no commas expected here
    unexpected_comma(term);
    if (!process_long_switch(term)) {
      // The special switches are differentiated by their last character
      auto control_char = term.back();
      term.pop_back();
      switch(control_char) {
      case ':':
        from = stospace(term);
        break;
      case '!':
        target(stospace(move(term)));
        break;
      case '.':
        process_short_switches(term);
        break;
      case 'h':
      case 'H':
        // This input is in the hexedecimal format
        makesure_empty();
        if (parse_hex(term, &data[0], alpha))
          return pop_data(colorspaces::rgb);
        [[fallthrough]];
      default:
        term.push_back(control_char);
        processor::silent_operate(term);
        return true;
      }
    }
  }
  return false;
}

bool interface::silent_add_term(const string& terms, const string& sep) {
  tstring ts(terms);
  output_stream.str("");
  for(tstring token; !(token = get_word(ts)).empty();) {
    if (silent_add_term(token)) {
      output_stream << sep;
    }
  }
  return true;
}

GLOBAL_NAMESPACE_END
