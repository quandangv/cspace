#include <iostream>

#include "conversion.hpp"
#include "parse.hpp"

int main(int argc, char** argv) {
  colorspace from = colorspaces::rgb,
               to = colorspaces::rgb;
  for(int i = 0; i < argc; i++) {
    switch(*argv[i]) {
    case '>':
      from = stospace(argv[i]+1);
      break;
    case '<':
      to = stospace(argv[i]+1);
      break;
    default:
      break;
    }
  }
}
