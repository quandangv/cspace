# cspace
A C++ color tool and library for handling colors, including color space conversion between `RGB`, `HSV/HSL`, `XYZ`, `CIELab` and the new `Jzazbz` color space.

Also included is the _color modification_ utility that allow you to quickly change brightness and saturation of colors.

## Conversion functions
If you come just to find the code for color conversion, they're in `src/conversion`. Each file in the folder contains the conversion functions for the corresponding color space. The files are have no external dependency and can be easily appropriated for your code.

## Building the project
### Using build.sh
Run `./build.sh -A` to build and install the project with default settings. To also build and run tests, use `./build.sh -tA`.
### Manually
Cspace is installed using CMake by runnning
```
mkdir -p build && cd build
cmake .. # cmake -DBUILD_TESTS=ON .. to build tests
cmake --build .
sudo make install
```

## Using the library
The installation paths for the library and include files should be printed after the installation have completed. The library and executable files are also built into the folder `build/src`
