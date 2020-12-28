#include "interface.hpp"
#include "test.h"

#include <tuple>
#include <vector>
#include <sstream>

#include "token_iterator.hpp"

using namespace std;
using namespace cspace;
struct TestSet {
  string terms_str, result_str;
  void check(interface& item) const {
    item.separator = "-";
    token_iterator it;
    it.set_input(terms_str);
    
    // feed terms to the interface and check the result
    string real_result = "";
    for(; it.next_token();) {
      auto r = item.add_term(string(it.token()));
      if (!r.empty())
        real_result += r + " ";
    }
    EXPECT_EQ(real_result, result_str);
  }
};

class GetTest : public ::testing::Test, public ::testing::WithParamInterface<TestSet> {};

vector<TestSet> interface_tests = {
  {"0.1 0.2 0.3, 0.4 0.5 0.6 0.7", "1A334D66 8099B3 "},
  {"HSV! rgb: 1 0 0 0 1 1 0.69", "0-1-1 180-1-1 "},
  {"1 2 3 hsv! 1 1 2", "FFFFFF 240-0.5-1 "},
  {"hsv! 0 1 1 swap 120 1 1", "180-1-1 0-1-0 "},
  {"cmyk! 0.5 0.25 1", "0.5-0.75-0-0 "},
  {"cmyk! 0.2, 0.5, 0.25, 1", "0.2-0.5-0.75-0-0 "},
  {"hex: on hsv: 150 0.75 0.75", "30BF78 "},
  {"hsv! xyz: 00ffffh", "180-1-1 "},
  {"axxx hsv! xyz: 8000FFFFh", "0.501961-180-1-1 "},
  {"xxxa hsv! xyz: 00FFFF80h", "180-1-1-0.501961 "},
  {"xxxa hsv: 180 1 1, 0.5  60 0.5 0.5, 0.2 0 1 1 0.69", "00FFFF80 80804033 FF0000 "},
  {"inter: hsv hex: on mod: 'value * 2, h +60'  002040h", "400080 "},
  {"inter: hsv hex: on mod: 'value* 2 h +60'  002040h", "400080 "},
  {"inter: hsv hex: on mod: 'value = 0.75'  002040h", "0060BF "},
  {"inter: hsv hex: on mod: 'value=0.75'  002040h", "0060BF "},
  {"p. 3 0.987654 0.123456 0.654987  0.1 1 0.123456", "FC1FA7 1AFF1F "},
};
INSTANTIATE_TEST_SUITE_P(Interface, GetTest, ::testing::ValuesIn(interface_tests));

TEST_P(GetTest, simple) {
  interface it;
  GetParam().check(it);
}
