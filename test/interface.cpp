#include "interface.hpp"

#include <tuple>
#include <vector>
#include <sstream>

#include "test.hpp"

using namespace std;
struct TestSet {
  string terms_str, result_str, expected_str, not_expected_str;
  vector<string> split_words(const string& value) const {
    vector<string> result;
    stringstream ss(value);
    while(ss.good()) {
      string term;
      ss >> term;
      if (!term.empty()) {
        if (term == "_")
          term = "";
        result.emplace_back(move(term));
      }
    }
    return move(result);
  }
  void check(interface& item) const {
    item.clear();
    item.separator = "-";

    auto terms = split_words(terms_str);
    auto expected = split_words(expected_str);
    auto not_expected = split_words(not_expected_str);
    
    // feed terms to the interface and check the result
    string real_result = "";
    for(int i = 0; i < terms.size(); i++) {
      auto r = item.add_term(string(terms[i]));
      if (!r.empty())
        real_result += r + "\n";
    }
    EXPECT_EQ(real_result, result_str);

    auto state = item.get_terms();
    auto final_terms = split_words(state);

    // check if the list of terms contains unexpected terms or not contain the expected
    auto contain = [&](string item)->bool {
      for (auto& term : final_terms)
        if (term == item)
          return true;
      return false;
    };
    for(auto& term : expected)
      EXPECT_TRUE(contain(term)) << "Expected term '" << term << "' in result '" << state << "' of test set: " << terms_str;
    for(auto& term : not_expected)
      EXPECT_FALSE(contain(term)) << "Didn't expect term '" << term << "' in result '" << state << "' of test set";
  }
};

class GetTest : public ::testing::Test, public ::testing::WithParamInterface<TestSet> {};

vector<TestSet> interface_tests = {
  {{"xyz! hsv: cielab! rgb:"}, {""}, {"cielab! rgb:"}, {"xyz! hsv:"}},
  {{"0.1 0.2 0.3"}, {"0.1-0.2-0.3\n"}, {}, {"0.1 0.2 0.3"}},
  {{"rgb: HSV! 1 0 0 0 1 1 0.69"}, {"0-1-1\n180-1-1\n"}, {"rgb: hsv! 0.69"}, {"1 0"}},
  {{"c. on 1 2 3 hsv! 1 1 2"}, {"1-1-1\n240-0.5-1\n"}, {}, {"2 3"}},
  {{"hex! hsv: 150 0.75 0.75"}, {"30BF78\n"}, {"rgb!"}, {"rgb:"}},
  {{"xyz: hsv! 00ffffh"}, {"180-1-1\n"}, {"xyz: hsv!"}, {}},
  {{"axxx! hsv! xyz: 8000FFFFh"}, {"0.501961-180-1-1\n"}, {"xyz: hsv!"}, {}},
  {{"xxxa! hsv! xyz: 00FFFF80h"}, {"180-1-1-0.501961\n"}, {"xyz: hsv!"}, {}},
  {{"xxxa! hex! hsv: alpha: on 180 1 1 0.5  60 0.5 0.5 0.2 alpha: off 0 1 1 0.69"}, {"00FFFF80\n80804033\nFF0000\n"}, {"hsv: rgb! 0.69"}, {}},
  {{"p. 3 0.987654 0.123456 0.654987 0.1 1 0.123456"}, {"0.988-0.123-0.655\n0.1-1-0.123\n"}, {}, {}},
};
INSTANTIATE_TEST_SUITE_P(Interface, GetTest, ::testing::ValuesIn(interface_tests));

TEST_P(GetTest, simple) {
  interface it;
  GetParam().check(it);
}
