#pragma once

#include <stdexcept>

namespace cspace {
  class error_base : public std::runtime_error { using std::runtime_error::runtime_error; };
}

