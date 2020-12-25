#pragma once

#include <string>

using std::string;

struct token_iterator {
  token_iterator() {}
  token_iterator(string&& s) : input(move(s)), position(0) {}
  
  string input;
  size_t position;
  size_t mark;

  token_iterator& set_input(string, size_t = 0);
  bool next_token();
  token_iterator& return_token(size_t from);
  const string& token() const;
  template<int (*F)(int)> bool next_token_base();

private:
  string m_token;

};

#include <cstring>
#include "logger.hpp"

template<int (*F)(int)>
bool token_iterator::next_token_base() {
  while(true) {
    if (input.empty() || position == string::npos) {
      return false;
    } else {
      constexpr const char space_chars[] = " \t\r\n\v\f";
      constexpr const char quote_chars[] = "'\"";

      // Find the start of the word
      mark = input.find_first_not_of(space_chars, position);
      if (mark == string::npos)
        return false;

      if (strchr(quote_chars, input[mark]) != nullptr) {
        position = input.find(input[mark], mark + 1);
        mark++;
        if (position == string::npos) {
          m_token = input.substr(mark);
          logger::warn("Unterminated string: " + m_token);
        } else {
          m_token = input.substr(mark, position - mark);
          position++;
        }
        return true;
      } else {
        for(position = mark+1; position < input.size(); position++)
          if (!F(input[position]))
            break;
        if (position == string::npos) {
          m_token = input.substr(mark);
        } else
          m_token = input.substr(mark, position - mark);
        return true;
      }
    }
  }
}
