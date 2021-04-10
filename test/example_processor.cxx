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
    std::cout << "Input: ";
    std::cin >> code;
    
    std::cout << "Multiply brightness by: ";
    std::cin >> p.modifications.back().value;
    auto result = p.operate_hex(code);
    std::cout << "Resulting color: " << result << std::endl;
  }
}
