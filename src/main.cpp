#include <iostream>

#include "token_iterator.hpp"
#include "interface.hpp"
#include "logger.hpp"

using namespace std;
using namespace cspace;
constexpr const char scope[] = "main";

int main(int argc, char** argv) {
  interface intf;

  // Read the next term to feed to the interface
  auto read_term = [&]()->string {
    // First, read from the arguments
    static int argi = 1;
    if (argi < argc)
      return string(argv[argi++]);
    else if (argi == argc) {
      if (!intf.stay) {
        // If we aren't told to stay, quit when the argument runs out.
        intf.quit = true;
        return "";
      } else
        argi++;
    }

    // Then, read terms from stdin
    static token_iterator it;
    while(!it.next_token()) {
      // If the iterator runs out of tokens, feed it the next line from stdin
      std::getline(std::cin, it.input);
      it.position = 0;
    }
    return it.token();
  };

  // Continually feed terms to the interface and prints the output
  while(!intf.quit) {
    try {
      auto result = intf.add_term(read_term());
      if (!result.empty())
        std::cout << result << std::endl;
    } catch (const exception& err) {
      logger::error(err.what());
    }
  }
  // Print a warning if there is unprocessed data remaining
  intf.makesure_empty();
}
