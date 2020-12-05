#include "parse.hpp"

#include <tuple>
#include <vector>

#include "test.hpp"
#include "option.hpp"

using namespace std;
using TestCompoments = tuple<int, component, component, component, component>;
using TestSet = pair<string, TestCompoments>;

class GetTest : public ::testing::Test, public ::testing::WithParamInterface<TestSet> {};

vector<TestSet> parse_tests = {
  {"#123", make_tuple(0xF, 0xF, 1, 2, 3)},
  {"#4321", make_tuple(0xF, 4, 3, 2, 1)},
  {"#123456", make_tuple(0xFF, 0xFF, 0x12, 0x34, 0x56)},
  {"#87654321", make_tuple(0xFF, 0x87, 0x65, 0x43, 0x21)},
  {"#123456789ABC", make_tuple(0xFFFF, 0xFFFF, 0x1234, 0x5678, 0x9ABC)},
  {"#FEDCBA9876543210", make_tuple(0xFFFF, 0xFEDC, 0xBA98, 0x7654, 0x3210)},
  {"#0123", make_tuple(15, 0, 0, 0, 0)},
  {"$123456", make_tuple(0, 0, 0, 0, 0)},
  {"#12345g", make_tuple(0, 0, 0, 0, 0)},
  {"#12346", make_tuple(0, 0, 0, 0, 0)},
};
INSTANTIATE_TEST_SUITE_P(Parse, GetTest, ::testing::ValuesIn(parse_tests));

TEST_P(GetTest, parse_code) {
  component a, r, g, b;
  auto expected = GetParam().second;
  EXPECT_EQ(parse_code(GetParam().first, a, r, g, b), get<0>(expected));
  if (get<0>(expected)) {
    EXPECT_EQ(get<1>(expected), a);
    EXPECT_EQ(get<2>(expected), r);
    EXPECT_EQ(get<3>(expected), g);
    EXPECT_EQ(get<4>(expected), b);
  }
}
