#include "token_iterator.hpp"

#include <string>

#include "logger.hpp"

// Set the string from which to extract tokens and the position to start extracting
token_iterator& token_iterator::set_input(string inp, size_t pos) {
  input = inp;
  position = pos;
  return *this;
}

int isntspace(int c) {
  return !std::isspace(c);
}

bool token_iterator::have_token() {
  position = input.find_first_not_of(space_chars, position);
  if (position == string::npos)
    return false;
  return true;
}

// Advance to the next token made up of non-space characters
bool token_iterator::next_token() {
  return next_token_base<isntspace>();
}

// Return part of the current token to be used for the next one
token_iterator& token_iterator::return_token(size_t from) {
  position = mark + from;
  return *this;
}

const string& token_iterator::token() const {
  return m_token;
}
