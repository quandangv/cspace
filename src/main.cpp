#include <iostream>

#include "conversion.hpp"
#include "parse.hpp"
#include "log.hpp"

constexpr const char scope[] = "main";

int main(int argc, char** argv) {
  double data[4];
  int count = 0;
  colorspace from = colorspaces::rgb,
               to = colorspaces::rgb;
  for(int i = 1; i < argc; i++) {
    auto term = string(argv[i]);
    if (term.empty()) continue;

    int len;
    if (sscanf(argv[i], "%lf%n", &data[count], &len) == 1 && len == term.size()) {
      if (++count == colorspaces::component_count(from)) {
        log::debug<scope>("Before conversion: " + to_string(&data[0], count) + ", from:" + to_string(from) + ", to: " + to_string(to));
        colorspaces::convert(&data[0], from, to);
        auto output = to_string(&data[0], to);
        std::cout << output << std::endl;
        count = 0;
      }
    } else {
      auto control_char = term.back();
      term.pop_back();
      switch(control_char) {
      case ':':
        from = stospace(move(term));
        break;
      case '!':
        to = stospace(move(term));
        break;
      default:
        log::error("Parsing: Unknown argument '"s + argv[i]);
        break;
      }
    }
  }
}
