#include "interface.hpp"

#include <iomanip>

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

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
    // Feed term eater
    // They are terms that takes in additional arguments
    feed_term_eater(move(term));
    unexpected_comma(term);
  } else if (parse(term.c_str(), data[data_count])) {
    // The term contains numerical data
    if (++data_count >= colorspaces::component_count(from)) {
      if (comma) {
        // The alpha component is present, add another component before we start the operation
        if (alpha) throw new interface_error("Add-term: Enough components have been received, excess comma at: " + term);
        else
          alpha = true;
      } else
        // Got enough components, start the operation
        return pop_data(from, to);
    }
    comma = false;
  } else {
    // Processing the switches, no commas expected here
    unexpected_comma(term);
    
    if (term == "--help" || term == "-h") {
      print_help();
    } else {
      if (!process_long_switch(term)) {
        // The term isn't a long switch, it's one of the special switches
        auto control_char = term.back();
        term.pop_back();
        switch(control_char) {
        case ':':
          from = stospace(term);
          break;
        case '!':
          // If any output color space is specified, disable hex output mode
          // It's only meant to output in RGB color space
          if (use_hex())
            use_hex(false);
          to = stospace(move(term));
          break;
        case '.':
          process_short_switches(term);
          break;
        case 'h':
        case 'H':
          // This is input in the hexedecimal format
          {
            // Replace data with components from the color code and start conversion
            component comp[4];
            auto divider = parse_code(term, &comp[0], alpha);
            if (divider != 0) {
              makesure_empty();
              for(int i = 0; i < 4; i++)
                data[i] = static_cast<double>(comp[i]) / divider;
              return pop_data(colorspaces::rgb, to);
            }
          }
          // If the hexedecimal parsing failed, fall through
        default:
          term.push_back(control_char);
          logger::error("Parsing: Unknown term '" + term + "'");
          break;
        }
      }
    }
  }
  return "";
}

