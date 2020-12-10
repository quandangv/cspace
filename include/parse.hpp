#pragma once

#include "option.hpp"
#include "colorspace.hpp"
#include "error.hpp"

DEFINE_ERROR(parse_error)

std::string to_string(const double* value, colorspace);
std::string to_string(const double* value, int count);

using component = unsigned int;
int parse_code(const std::string&, component* comps, bool& has_alpha);

bool parse(const char*, double&);
bool parse(const char*, int&);
bool parse(const char*, bool&);
