#include "interface.hpp"

#include <iomanip>

using std::cout;
using std::string;
using std::endl;

constexpr char scope[] = "interface";

std::string interface::add_term(string&& term) {
  logger::debug<scope>("Interface-Term: '" + term + "'");
  if (term.empty()) return "";
  if (!waiting_terms.empty()) {
    logger::debug<scope>("Interface: Feed waiting terms: "+term);
    feed_waiting_term(waiting_terms.back(), move(term));
    waiting_terms.pop_back();
  } else if (parse(term, data[count])) {
    if (++count == colorspaces::component_count(from) + (int)alpha) {
      if (alpha) {
        if (alpha_first) {
          alpha_val = data[0];
          for(int i = 1; i < count; i++)
            data[i-1] = data[i];
        } else alpha_val = data[count-1];
        count--;
      }
      return pop_data(from, to);
    }
  } else {
    
    // The term isn't data, check if it is a valid control term
    if (term == "--help") {
      print_help();
    } else if (!process_long_switch(term)) {
      auto control_char = term.back();
      term.pop_back();
      switch(control_char) {
      case ':':
        from = stospace(move(term));
        break;
      case '!':
        if (hex) {
          hex = false;
          output_stream << std::dec << std::setfill(' ');
        }
        to = stospace(move(term));
        break;
      case '.':
        logger::debug<scope>("Interface: single-character switches");
        process_short_switches(term);
        break;
      case 'h':
      case 'H':
        {
          unsigned int a, r, g, b;
          auto divider = parse_code(term, a, r, g, b, alpha_first);
          if (divider != 0) {
            makesure_empty();
            alpha_val = static_cast<double>(a) / divider;
            data[0] = static_cast<double>(r) / divider;
            data[1] = static_cast<double>(g) / divider;
            data[2] = static_cast<double>(b) / divider;
            return pop_data(colorspaces::rgb, to);
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
  return "";
}

