cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cspace VERSION 1.1.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(utils)

add_subdirectory(private-headers)
add_subdirectory(include)
add_subdirectory(src)

unset(DEBUG_SCOPES CACHE)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
