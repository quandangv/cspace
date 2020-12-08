#pragma once

#include "option.hpp"
#include "colorspace.hpp"
#include "error.hpp"

DEFINE_ERROR(parse_error)

std::string to_string(const double* value, colorspace);
std::string to_string(const double* value, int count);

using component = unsigned int;
int parse_code(const std::string&, component& alpha, component&, component&, component&, bool alpha_first = true);

bool parse(std::string, double&);
bool parse(std::string, int&);
bool parse(std::string, bool&);
