#include "interface.hpp"
#include "logger.hpp"

#include <iomanip>

#include "parse.hpp"

GLOBAL_NAMESPACE

DEFINE_ERROR(interface_error)
constexpr char scope[] = "interface";

string interface::add_data(double value) {
  data[data_count] = value;
  if (++data_count >= component_count(from)) {
    if (comma) {
      // The alpha component is present, add another component before we start the operation
      if (alpha) throw new interface_error("Add-term: Enough components have been received, excess comma");
      alpha = true;
    } else
      // Got enough components, start the operation
      return pop_data(from);
  }
  comma = false;
  return "";
}

// Process the given term
// If the term triggers an operation, the resulting string is returned
std::string interface::add_term(string&& term) {
  logger::debug<scope>("Interface-Term: '" + term + "'");
  unexpected_comma(term);
  if (term.empty()) return "";
  if (term.back() == ',') {
    // Extract the comma from the end of the term
    // The comma is used to signify the presence of the alpha component
    term.pop_back();
    if (term.empty()) {
      logger::warn("Parsing term: Orphaned comma ',', put commas at the end of a term without spaces");
      return "";
    } else
      comma = true;
  }

  if (eater != nullptr) {
    // Feed term eater, which are terms that takes in additional arguments
    feed_term_eater(move(term));
    unexpected_comma(term);
  } else if (double value; parse(term.c_str(), value)) {
    return add_data(value);
  } else {
    // Processing the switches, no commas expected here
    unexpected_comma(term);
    if (term.front() == '#') {
      makesure_empty();
      term.erase(0, 1);
      if (parse_hex(term, &data[0], alpha))
        return pop_data(colorspaces::rgb);
    }
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
        // If the hexedecimal parsing fail, fall through
      default:
        term.push_back(control_char);
        logger::error("Parsing: Unknown term '" + term + "'");
        break;
      }
    }
  }
  return "";
}

GLOBAL_NAMESPACE_END
