#include "test.hpp"
#include "conversion.hpp"

using namespace std;

constexpr double tolerance=0.01;

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
  if (expect_near(test, control, #a, #b, a##_str)) { \
    load_data(a##_str, test); \
    a##_##b(test, test); \
    expect_near(test, control, #a, string(#b) + " (in-place)", a##_str); \
  }
#define TEST_ROUTE(a, b) TEST_METHOD(a, b) TEST_METHOD(b, a)
		TEST_ROUTE(rgb, xyz)
		TEST_ROUTE(rgb, hsl)
		//TEST_ROUTE(rgb, hsv)
		TEST_ROUTE(cielab, xyz)
#undef TEST_ROUTE
#undef TEST_METHOD
  }
  
  bool is_near(const double d1[3], const double d2[3], double tolerance) const {
       return std::abs(d1[0] - d2[0]) <= tolerance &&
              std::abs(d1[1] - d2[1]) <= tolerance &&
              std::abs(d1[2] - d2[2]) <= tolerance;
  }
  
  bool expect_near(const double output[3], const double expected[3], string from, string to, string input) {
    auto result = is_near(output, expected, tolerance);
    EXPECT_TRUE(result) << "Conversion from " << from << " to " << to << " is wrong, input: " << input << "\nOutput: " << to_string(output) << "\nExpected: " << to_string(expected) << "\nOriginal RGB: " << rgb_str << endl;
    return result;
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
    test_set{"1 1 1", "0 0 1", "0 0 1", ".95047 1 1.08883", "100 0 0"},
    test_set{"1 0 0", "0 1 0.5", "0 1 1", ".412456 .212673 .019334", "53.24079 80.09249 67.20319"},
    test_set{"0.5 0.25 1", "260 1 0.625", "260 0.75 1", ".286912 .154080 .960506", "46.18801 67.35635 -84.59172"},
  };

  for (auto test : tests) {
    test.test();
  }
}

