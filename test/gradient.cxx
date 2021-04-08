#include "test.h"
#include "gradient.hpp"

using namespace std;
using namespace cspace;

gradient<1> g;

TEST(Gradient, auto_add) {
  g.points[0][0] = 10;
  g.points[4.5][0] = 11;
  g.points[10][0] = 10;
  g.auto_add(8);
  EXPECT_EQ(g.points.size(), 11);
  EXPECT_EQ(g.points.upper_bound(0)->first, 0.9);
  EXPECT_EQ(g.points.upper_bound(4.5)->first, 5.6);
  EXPECT_EQ(g.points.upper_bound(0)->second[0], 10.2);
  EXPECT_EQ(g.points.upper_bound(4.5)->second[0], 10.8);
}

TEST(Gradient, get) {
  double tmp;
  g.get(0.25, &tmp);
  EXPECT_EQ(tmp, 10 + 1.0/18);
}

