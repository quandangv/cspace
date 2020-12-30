#include "processor.hpp"
#include "test.hpp"

using namespace cspace;

TEST(Processor, operate) {
  double data[] = {0, 1, 1, 0.5};
  double* const ptr = &data[0];
  processor p;
  p.alpha_first = false;
  p.target(colorspaces::hsv);
  EXPECT_EQ("120 1 1 0.501961", p.operate("#00FF0080"));
  EXPECT_EQ("120 1 1 0.5", p.operate("rgb(0, 1, 0, 0.5)"));
  EXPECT_EQ("120 1 1 0.5", p.operate("hsl(120, 1, 0.5, 0.5)"));
  EXPECT_EQ("180 1 1 0.5", p.operate(ptr, true, colorspaces::rgb));
  data[0] = 0;
  data[1] = 1;
  data[2] = 1;
  data[3] = 0.5;
  EXPECT_EQ("180 1 1", p.operate(ptr, false, colorspaces::rgb));
  data[0] = 0.1;
  data[1] = 0.2;
  data[2] = 0.3;
  data[3] = 0.5;
  p.target(colorspaces::rgb);
  p.inter = colorspaces::hsl;
  p.add_modification("lightness * 2");
  EXPECT_EQ("0.2 0.4 0.6 0.5", p.operate(ptr, true, colorspaces::rgb));
}
