#include <cspace/conversion.hpp>
#include <iostream>

int main() {
  std::cout << "Enter RGB color in format 'R G B', each component within range [0-1].\n";
  while(true) {
    double rgb[3];
    std::cout << "Input: ";
    std::cin >> rgb[0] >> rgb[1] >> rgb[2];
    cspace::rgb_hsv(&rgb[0], &rgb[0]);
    std::cout << "Color converted to HSV: " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << std::endl;
  }
}
