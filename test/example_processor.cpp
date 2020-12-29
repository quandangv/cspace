#include <cspace/processor.hpp>
#include <iostream>
#include <string>

int main() {
  cspace::processor p;
  std::cout << "Enter RGB in hexedecimal color code.\n";
  p.modifications.push_back(cspace::mod("lightness * 1", cspace::colorspaces::cielab));
  p.inter = cspace::colorspaces::cielab;
  while(true) {
    std::string code;
    double data[4];
    double* const ptr = &data[0];
    bool have_alpha;
    
    std::cout << "Input: ";
    std::cin >> code;
    cspace::parse_hex(code, ptr, have_alpha);
    
    std::cout << "Multiply brightness by: ";
    std::cin >> p.modifications.back().value;
    auto result = p.operate(ptr, have_alpha, cspace::colorspaces::rgb);
    std::cout << "Resulting color: " << result << std::endl;
  }
}
