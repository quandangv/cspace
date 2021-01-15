#include <iostream>

#include "interface.hpp"
#include "logger.hpp"

using namespace std;
using namespace cspace;
constexpr const char scope[] = "main";

int main(int argc, char** argv) {
  interface intf;
  for(int i = 1; i < argc && !intf.quit; i++) {
    try {
      auto result = intf.add_term(argv[i]);
      if (!result.empty())
        std::cout << result << std::endl;
    } catch (const exception& e) {
      logger::error(e.what());
    }
  }
  if (intf.stay) {
    while(!intf.quit) {
      try {
        string line; std::getline(cin, line);
        auto result = intf.add_multiple_terms(line);
        if (!result.empty())
          std::cout << result << std::endl;
      } catch (const exception& e) {
        logger::error(e.what());
      }
    }
  }
  // Print a warning if there is unprocessed data remaining
  intf.makesure_empty();
}
