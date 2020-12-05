#include <iostream>

#include "conversion.hpp"
#include "interface.hpp"
#include "parse.hpp"
#include "log.hpp"

constexpr const char scope[] = "main";

int main(int argc, char** argv) {
  interface processor;
  auto read_word = [&]()->string {
    static int argi = 1;
    if (argi < argc)
      return string(argv[argi++]);
      
    static string input = string().c_str();
    static size_t pos = 0;
    while(true) {
      if (input.empty()) {
        std::getline(std::cin, input);
        pos = 0;
      } else {
        constexpr const char space_chars[] = " \t";
        auto start = input.find_first_not_of(space_chars, pos);
        if (start == string::npos) {
          input.clear();
          continue;
        }
        pos = input.find_first_of(space_chars, start);
        if (pos == string::npos) {
          auto result = input.substr(start);
          input.clear();
          return result;
        } else {
          return input.substr(start, pos - start);
        }
      }
    }
  };
  while(true) {
    auto result = processor.take_term(read_word());
    if (!result.empty())
      std::cout << result << std::endl;
  }
}
