#pragma once

#include <string>
#include <cstring>

#include "tstring.hpp"

using std::string;

constexpr const char space_chars[] = " \t\r\n\v\f";
constexpr const char quote_chars[] = "'\"";

template<int (*char_func)(int)>
bool get_token(const string& input, size_t& position, tstring& token) {
  while(true) {
    if (input.empty() || position == tstring::npos) {
      return false;
    } else {
      // Find the start of the token, ignoring spaces
      position = input.find_first_not_of(space_chars, position);
      if (position == tstring::npos)
        return false;

      if (strchr(quote_chars, input[position]) != nullptr) {
        // Parse terms enclosed in quotes
        auto start = position + 1;
        position = input.find(input[position], start);
        if (position == tstring::npos) {
          token = tstring(input.data(), start, input.size());
          // WARNING: Unterminated quotes
        } else {
          token = tstring(input.data(), start, position);
          position++;
        }
        return true;
      } else {
        // Extend the length of the term until we hit an unaccepted character
        auto start = position;
        for(position = start + 1; position < input.size(); position++)
          if (!char_func(input[position]))
            break;
        token = tstring(input.data(), start, position);
        return true;
      }
    }
  }
}

class token_iterator {
public:
  token_iterator(string&& s) : input(move(s)), position(0) {}
  token_iterator() : token_iterator("") {}

  string input;
  size_t position;
  tstring token;

  token_iterator& set_input(string&&, size_t position = 0);
  bool have_token();
  bool next_token();
  template<int (*F)(int)> bool next_token_base() { return get_token<F>(input, position, token); }
  static int isntspace(int c) { return !std::isspace(c); }
};
