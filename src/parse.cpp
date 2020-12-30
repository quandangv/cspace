#include "parse.hpp"
#include "logger.hpp"

#include <sstream>
#include <iomanip>

GLOBAL_NAMESPACE

DEFINE_ERROR(parse_error)

using namespace std;

bool parse(const tstring& str, double& result) {
  int len;
  return sscanf(str.begin(), "%lf%n", &result, &len) == 1 && len == str.size();
}

bool parse(const tstring& str, int& result) {
  int len;
  return sscanf(str.begin(), "%d%n", &result, &len) == 1 && len == str.size();
}

bool parse(const tstring& str, bool& result) {
  #define CMP(a) strncasecmp(str.begin(), #a, str.size()) == 0
  if (CMP(on) || CMP(true))
    result = true;
  else if (CMP(off) || CMP(false))
    result = false;
  else return false;
  return true;
  #undef CMP
}

GLOBAL_NAMESPACE_END
