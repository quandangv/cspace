# cspace
A C++ color tool and library for handling colors, including color space conversion between `RGB`, `HSV/HSL`, `XYZ`, `CIELab` and the new `Jzazbz` color space.

Also included is the _color modification_ utility that allow you to quickly change brightness and saturation of colors.

## Conversion functions
If you come just to find the code for color conversion, they're in `src/conversion`. Each file in the folder contains the conversion functions for the corresponding color space. The files are have no external dependency and can be easily appropriated for your code.

## Building the project
Run `./build.sh -A` in the project root to build and install the project with default settings. To view other build options, run `./build.sh -h`.

## Using the tool
Here are some example commands using cspace:
- Convert `#FF00FF` to CIELab color space

  `cspace cielab! '#FF00FF'`
  
- Increase color brightness 1.5 times

  `cspace mod: 'brightness * 2' '#FF0000'`
  
For more information, view the help text by running `cspace --help`

## Using the library
The installation paths for the library and include files should be printed after the installation have completed. The library and executable files are also built into the folder `build/src`

Here is an example of a simple program using the conversion functions
```
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
```
More examples using the library can be found in the `test` folder.
