cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(cscheme VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(mode.hpp.in mode.hpp)
target_include_directories(cscheme PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

