#include "test.hpp"
#include "conversion.hpp"

using namespace std;

constexpr double tolerance=0.00001;

void load_data(string data, double arr[3]) {
  string::size_type sz;
  char* ptr = &(data[0]);
  arr[0] = std::strtod(ptr, &ptr);
  arr[1] = std::strtod(ptr, &ptr);
  arr[2] = std::strtod(ptr, &ptr);
}

string to_string(const double arr[3]) {
  return to_string(arr[0]) + " " + to_string(arr[1]) + " " + to_string(arr[2]);
}

struct test_set {
  string rgb_str, hsl_str, hsv_str, xyz_str, cielab_str;

  void test() {
    double control[3], test[3];
#define TEST_METHOD(a, b) \
  load_data(a##_str, control); \
  a##_##b(control, test); \
  load_data(b##_str, control); \
  expect_near(test, control, #a, #b, a##_str); \
  load_data(a##_str, test); \
  a##_##b(test, test); \
  expect_near(test, control, #a, string(#b) + " (in-place)", a##_str);
#define TEST_ROUTE(a, b) TEST_METHOD(a, b) TEST_METHOD(b, a)
		TEST_ROUTE(rgb, xyz)
		TEST_ROUTE(rgb, hsl)
		TEST_ROUTE(rgb, hsv)
		TEST_ROUTE(cielab, xyz)
#undef TEST_ROUTE
#undef TEST_METHOD
  }
  
  bool is_near(const double d1[3], const double d2[3], double tolerance) const {
       return std::abs(d1[0] - d2[0]) <= tolerance &&
              std::abs(d1[1] - d2[1]) <= tolerance &&
              std::abs(d1[2] - d2[2]) <= tolerance;
  }
  
  void expect_near(const double output[3], const double expected[3], string from, string to, string input) {
    EXPECT_TRUE(is_near(output, expected, tolerance)) << "Conversion from " << from << " to " << to << " is wrong, original RGB: " << rgb_str << endl << "Input: " << input << "\nOutput: " << to_string(output) << "\nExpected: " << to_string(expected) << endl;
  }
};

TEST(self, load) {
  double tmp[3]; load_data("0.321 159 200", tmp);
  EXPECT_EQ(0.321, tmp[0]);
  EXPECT_EQ(159, tmp[1]);
  EXPECT_EQ(200, tmp[2]);
}

TEST(conversion, hsl_rgb_xyz_jab) {
  test_set tests[] {
    test_set{"0 0 0", "0 0 0", "0 0 0", "0 0 0", "0 0 0"},
    test_set{"1 1 1", "0 0 1", "0 0 1", "95.04285453771806 100 108.89003707981276", "100 0 0"},
    test_set{"1 0 0", "0 1 0.5", "0 1 1", "41.23865632529916 21.26368216773238 1.9330620152483982", "53.23288178584245 80.10930952982204 67.22006831026425"},
    test_set{"0.4980392156862745 0.24705882352941178 1", "260.000000 1.000000 0.623529", "260 0.752942 1", "28.574624890893368 15.285756027773825 96.0400020841283", "46.02481292771143 67.62529868187306 -84.87406075111976"},
  };

  for (auto test : tests) {
    test.test();
  }
}

