#include "colorspace.hpp"
#include "test.hpp"

#include <vector>

#include "parse.hpp"

using namespace std;
using namespace cspace;

struct TestCompoments {
  bool has_alpha;
  double divider;
  vector<unsigned int> comp;
};
using TestSet = pair<string, TestCompoments>;

class GetTest : public ::testing::Test, public ::testing::WithParamInterface<TestSet> {};

vector<TestSet> parse_tests = {
  {"123", {false, 0xF, {1, 2, 3}}},
  {"4321", {true, 0xF, {4, 3, 2, 1}}},
  {"123456", {false, 0xFF, {0x12, 0x34, 0x56}}},
  {"87654321", {true, 0xFF, {0x87, 0x65, 0x43, 0x21}}},
  {"123456789ABC", {false, 0xFFFF, {0x1234, 0x5678, 0x9ABC}}},
  {"FEDCBA9876543210", {true, 0xFFFF, {0xFEDC, 0xBA98, 0x7654, 0x3210}}},
  {"0123", {true, 15, {0, 1, 2, 3}}},
  {"12345g", {false, 0, {}}},
  {"12346", {false, 0, {}}},
};
INSTANTIATE_TEST_SUITE_P(Parse, GetTest, ::testing::ValuesIn(parse_tests));

TEST_P(GetTest, parse_hex) {
  double comp[4];
  bool has_alpha;
  auto expected = GetParam().second;
  auto success = parse_hex(GetParam().first, &comp[0], has_alpha);
  EXPECT_EQ(success, expected.divider != 0);
  if (success) {
    for (int i = 0; i < expected.comp.size(); i++)
      EXPECT_EQ(expected.comp[i] / expected.divider, comp[i]);
    EXPECT_EQ(expected.has_alpha, has_alpha);
  }
}
