#include "conversion.hpp"
#include "test.hpp"

#include <tuple>
#include <vector>

#include "colorspace.hpp"

using namespace std;
using namespace cspace;

constexpr double tolerance=0.0001;

struct color {
  double comp[4];
};

struct TestSet {
  color rgb, cmyk, hsl, hsv, xyz, cielab;
};

class GetTest : public ::testing::Test, public ::testing::WithParamInterface<TestSet> {};

vector<TestSet> conversion_tests{
  {{0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
  {{1, 1, 1}, {0, 0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {.95047, 1, 1.08883}, {1, 0, 0}},
  {{0.5, 0.5, 0.5}, {0, 0, 0, 0.5}, {0, 0, 0.5}, {0, 0, 0.5}, {.2034397, 0.2140411, 0.2330544}, {0.53389, 0, 0}},
  {{1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0.5}, {0, 1, 1}, {.412456, .212673, .019334}, {0.5324079, 0.8009249, 0.6720319}},
  {{0.5, 0.25, 1}, {0.5, 0.75, 0, 0}, {260, 1, 0.625}, {260, 0.75, 1}, {.286912, .154080, .960506}, {0.4618801, 0.6735635, -0.8459172}},
};
INSTANTIATE_TEST_SUITE_P(Conversion, GetTest, ::testing::ValuesIn(conversion_tests));

string to_string(const double* arr, int count) {
  stringstream result;
  result  << arr[0];
  for(int i = 1; i < count; i++)
    result << ' ' << arr[i];
  return result.str();
}

bool expect_near(const double* output, const double* expected, const double* input, const double* rgb, string from, string to, string msg = "") {
  auto result = true;
  auto out_space = stospace(to.c_str());
  for(int i = 0; i < component_count(out_space); i++)
    if (std::abs(output[i] - expected[i]) > tolerance)
      result = false;

  EXPECT_TRUE(result) << "Conversion from " << from << " to " << to << " is wrong, input: " << to_string(input, stospace(from.c_str())) << "\nOutput: " << to_string(output, out_space) << "\nExpected: " << to_string(expected, out_space) << "\nOriginal RGB: " << to_string(rgb, 3) << endl << msg << endl;
  return result;
}

TEST_P(GetTest, direct) {
#define TEST_METHOD(a, b) \
  a##_##b(test_set.a.comp, test.comp); \
  if (expect_near(test.comp, test_set.b.comp, test_set.a.comp, test_set.rgb.comp, #a, #b)) { \
    test = test_set.a; \
    a##_##b(test.comp, test.comp); \
    expect_near(test.comp, test_set.b.comp, test_set.a.comp, test_set.rgb.comp, #a, #b, "In place test failed"); \
  }
#define TEST_ROUTE(a, b) TEST_METHOD(a, b) TEST_METHOD(b, a)
  color test;
  auto test_set = GetParam();
  TEST_ROUTE(rgb, xyz)
  TEST_ROUTE(rgb, hsl)
  TEST_ROUTE(rgb, hsv)
  TEST_ROUTE(rgb, cmyk)
  TEST_ROUTE(cielab, xyz)
#undef TEST_ROUTE
#undef TEST_METHOD
}

TEST_P(GetTest, indirect) {
  color test, expected;
  auto test_set = GetParam();
#define TEST_METHOD(a, b, compare) \
  test = test_set.a; \
  convert(test.comp, colorspaces::a, colorspaces::b); \
  // convert to a uniform colorspace to compare colors accurately \
  convert(test.comp, colorspaces::b, colorspaces::compare); \
  expected = test_set.b; \
  convert(expected.comp, colorspaces::a, colorspaces::compare); \
  expect_near(test.comp, expected.comp, test_set.a.comp, test_set.rgb.comp, #a, #b);
#define TEST_ROUTE(a, b, cmp) TEST_METHOD(a, b, cmp) TEST_METHOD(b, a, cmp)
  TEST_ROUTE(hsv, hsl, rgb)
  TEST_ROUTE(hsv, cielab, xyz)
  TEST_ROUTE(hsv, xyz, xyz)
  TEST_ROUTE(rgb, xyz, xyz)
#undef TEST_ROUTE
#undef TEST_METHOD
}
