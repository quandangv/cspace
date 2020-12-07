#pragma once

#include "option.hpp"
#include "colorspace.hpp"
#include "error.hpp"

DEFINE_ERROR(parse_error)

std::string to_string(const double* value, colorspace);
std::string to_string(const double* value, int count);
std::string to_string(colorspace);

colorspace stospace(std::string&&);

using component = unsigned int;
int parse_code(const std::string&, component& alpha, component&, component&, component&, bool alpha_first = true);
