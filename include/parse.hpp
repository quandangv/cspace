#pragma once

#include "option.hpp"
#include "colorspace.hpp"
#include "error.hpp"

DEFINE_ERROR(parse_error)

string to_string(const double* value, int count);

colorspace stospace(const char*);

using component = unsigned int;
int parse_code(const string&, component& alpha, component&, component&, component&);
