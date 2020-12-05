#pragma once

#include "option.hpp"
#include "colorspace.hpp"
#include "error.hpp"

DEFINE_ERROR(parse_error)

string to_string(const double* value, colorspace);
string to_string(colorspace);

colorspace stospace(string&&);

using component = unsigned int;
int parse_code(const string&, component& alpha, component&, component&, component&);
