cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(cscheme VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

