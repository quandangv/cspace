#include "interface.hpp"

#include <tuple>
#include <vector>
#include <sstream>

#include "test.hpp"

using namespace std;
struct TestSet {
  vector<string> terms, result, expected, not_expected;
  void check(interface& item) const {
    item.clear();
    EXPECT_EQ(result.size(), terms.size()) << "The number of terms and expected results have to match";
    // feed terms to the interface and check the result
    for(int i = 0; i < terms.size(); i++)
      EXPECT_EQ(result[i], item.add_term(string(terms[i])));
    
    // build the list of terms from the interface
    auto state = item.get_terms();
    vector<string>final_terms;
    stringstream ss(state);
    while(ss.good()) {
      final_terms.emplace_back();
      ss >> final_terms.back();
    }

    // check if the list of terms contains unexpected terms or not contain the expected
    auto contain = [&](string item)->bool {
      for (auto& term : final_terms)
        if (term == item)
          return true;
      return false;
    };
    for(auto& term : expected)
      EXPECT_TRUE(contain(term)) << "Expected term '" << term << "' in result '" << state << "' of test set";
    for(auto& term : not_expected)
      EXPECT_FALSE(contain(term)) << "Didn't expect term '" << term << "' in result '" << state << "' of test set";
  }
};

class GetTest : public ::testing::Test, public ::testing::WithParamInterface<TestSet> {};

vector<TestSet> interface_tests = {
  {{"xyz!", "hsv:", "cielab!", "rgb:"}, {"","","",""}, {"cielab!", "rgb:"}, {"xyz!", "hsv:"}},
  {{"0.1", "0.2", "0.3"}, {"", "", "0.1 0.2 0.3"}, {}, {"0.1", "0.2", "0.3"}},
  {{"rgb:", "HSV!", "1", "0", "0", "0", "1", "1", "0.69"}, {"", "", "", "", "0 1 1", "", "", "180 1 1", ""}, {"rgb:", "hsv!", "0.69"}, {"1", "0"}},
  {{"c.", "1", "2", "3", "hsv!", "1", "1", "2"}, {"", "", "", "1 1 1", "", "", "", "240 0.5 1"}, {}, {"2", "3"}},
};
INSTANTIATE_TEST_SUITE_P(Interface, GetTest, ::testing::ValuesIn(interface_tests));

TEST_P(GetTest, simple) {
  interface it;
  GetParam().check(it);
}
