cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(cscheme VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(include)
add_subdirectory(src)

if(TEST)
  enable_testing()
  add_subdirectory(test)
endif()

