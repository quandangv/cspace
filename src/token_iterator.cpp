#include "token_iterator.hpp"

#include <string>

#include "logger.hpp"

token_iterator& token_iterator::set_input(string inp, size_t pos) {
  input = inp;
  position = pos;
  return *this;
}

int isntspace(int c) {
  return !std::isspace(c);
}

bool token_iterator::next_token() {
  return  next_token_base<isntspace>();
}

token_iterator& token_iterator::return_token(size_t from) {
  position = mark + from;
  return *this;
}

const string& token_iterator::token() const {
  return m_token;
}
