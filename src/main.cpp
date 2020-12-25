#include <iostream>

#include "conversion.hpp"
#include "interface.hpp"
#include "parse.hpp"
#include "logger.hpp"
#include "token_iterator.hpp"

using namespace std;
constexpr const char scope[] = "main";

int main(int argc, char** argv) {
  interface processor;

  // This will return the next argument as a string
  // If all arguments have been returned, read from stdin and get the next word.
  auto read_word = [&]()->string {
    static int argi = 1;
    if (argi <= argc) {
      // If all arguments have been returned and stay switch is off, make the program quit
      if (argi++ == argc) {
        if (!processor.stay) {
          processor.quit = true;
          return "";
        }
      } else return string(argv[argi - 1]);
    }

    static token_iterator it;
    while(!it.next_token()) {
      std::getline(std::cin, it.input);
      it.position = 0;
    }
    return it.token();
  };

  // Continually feed terms to the interface;
  while(!processor.quit) {
    try {
      auto result = processor.add_term(read_word());
      if (!result.empty())
        std::cout << result << std::endl;
      logger::debug<scope>("End of a term, quit: " + to_string(processor.quit));
    } catch (const exception& err) {
      logger::error(err.what());
    }
  }
  processor.makesure_empty();
}
