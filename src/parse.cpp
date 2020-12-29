#include "parse.hpp"
#include "logger.hpp"

#include <sstream>
#include <iomanip>

GLOBAL_NAMESPACE

DEFINE_ERROR(parse_error)

using namespace std;

bool parse(const char* str, double& result) {
  int len;
  return sscanf(str, "%lf%n", &result, &len) == 1 && str[len] == 0;
}

bool parse(const char* str, int& result) {
  int len;
  return sscanf(str, "%d%n", &result, &len) == 1 && str[len] == 0;
}

bool parse(const char* str, bool& result) {
  #define CMP(a) strcasecmp(str, #a) == 0
  if (CMP(on) || CMP(true))
    result = true;
  else if (CMP(off) || CMP(false))
    result = false;
  else return false;
  return true;
  #undef CMP
}

GLOBAL_NAMESPACE_END
