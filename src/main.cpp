#include <iostream>

#include "conversion.hpp"
#include "parse.hpp"
#include "log.hpp"

int main(int argc, char** argv) {
  std::cout << "Hello world";
  double data[4];
  int count = 0;
  colorspace from = colorspaces::rgb,
               to = colorspaces::rgb;
  for(int i = 1; i < argc; i++) {
    switch(*argv[i]) {
    case '>':
      from = stospace(argv[i]+1);
      break;
    case '<':
      to = stospace(argv[i]+1);
      break;
    default:
      if (!sscanf(argv[i], "%d", &data[count])) {
        log::error("Parse-Value: Parse '" + string(argv[i]) + "' to double failed");
        continue;
      }
      if (++count == colorspaces::component_count(from)) {
        colorspaces::convert(&data[0], from, to);
        auto output = to_string(&data[0], to);
        std::cout << output << std::endl;
        log::debug("Main: Output result: " + output);
      }
      break;
    }
  }
}
