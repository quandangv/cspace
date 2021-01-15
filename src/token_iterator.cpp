#include "token_iterator.hpp"

#include <string>

// Set the string from which to extract tokens and the position to start extracting
token_iterator& token_iterator::set_input(string&& inp, size_t pos) {
  input = move(inp);
  position = pos;
  return *this;
}

bool token_iterator::have_token() {
  position = input.find_first_not_of(space_chars, position);
  if (position == string::npos)
    return false;
  return true;
}

int isntspace(int c) {
  return !std::isspace(c);
}

// Advance to the next token made up of non-space characters
bool token_iterator::next_token() {
  return get_token<isntspace>(input, position, token);
}
