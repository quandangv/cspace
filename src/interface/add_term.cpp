#include "interface.hpp"

#include <iomanip>

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

// Process the given term
// If the term triggers a conversion, return the resulting string
std::string interface::add_term(string&& term) {
  logger::debug<scope>("Interface-Term: '" + term + "'");
  if (comma) {
    logger::error("BUG: Coma left unconsumed, please report this");
    comma = false;
  }
  if (term.empty()) return "";
  if (term.back() == ',') {
    term.pop_back();
    if (term.empty()) {
      logger::warn("Parsing term: Orphaned comma ',', put commas at the end of a term without spaces");
      return "";
    } else
      comma = true;
  }

  if (!term_eater.empty()) {

    // Feed term eater
    feed_term_eater(move(term));
    if (comma) {
      logger::warn("Parsing term: Unexpected comma at: " + term);
      comma = false;
    }
  } else if (parse(term.c_str(), data.current())) {

    // Use term as numerical data
    if (++data.count == colorspaces::component_count(from) + (int)alpha) {
      // Got enough components, start the conversion
      auto data_ptr = &data[0];
      if (alpha)
        data.extract_alpha(alpha_first);
      return pop_data(data_ptr, from, to);
    } else
      comma = false;
  } else {
    
    // The term isn't data, check if it is a valid control term
    if (term == "--help") {
      print_help();
    } else {
      if (comma) {
        logger::warn("Parsing term: Unexpected comma at: " + term);
        comma = false;
      }
      if (!process_long_switch(term)) {
        auto control_char = term.back();
        term.pop_back();
        switch(control_char) {
        case ':':
          from = stospace(term);
          break;
        case '!':
          // If any output color space is specified, disable hex output mode,
          // which was only meant to output in RGB color space
          if (hex) {
            hex = false;
            output_stream << std::dec << std::setfill(' ');
          }
          to = stospace(move(term));
          break;
        case '.':
          process_short_switches(term);
          break;
        case 'h':
        case 'H':
          {
            // Replace data with components from the color code and start conversion
            unsigned int a, r, g, b;
            auto divider = parse_code(term, a, r, g, b, alpha_first);
            if (divider != 0) {
              makesure_empty();
              data.alpha = static_cast<double>(a) / divider;
              data[0] = static_cast<double>(r) / divider;
              data[1] = static_cast<double>(g) / divider;
              data[2] = static_cast<double>(b) / divider;
              return pop_data(&data[0], colorspaces::rgb, to);
            }
          }
          // fall through
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

