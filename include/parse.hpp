#pragma once

#include "option.hpp"
#include "conversion.hpp"
#include "error.hpp"

DEFINE_ERROR(parse_error)

string to_string3(const double* value);

colorspace stospace(const string&);

using component = unsigned int;
int parse_code(const string&, component& alpha, component&, component&, component&);
