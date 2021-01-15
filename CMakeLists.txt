cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cspace VERSION 1.1.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(cmake/targets)
include(file_list)

auto_targets(cspace "${HEADER_DIRS}" "${INCLUDE_DIR}" "${SOURCES}" "${HEADERS}" "${SRC_DIR}/main.cpp")

if(BUILD_TESTS)
  include(cmake/test)
  add_unit_tests(${TEST_DIR} "${INTERNAL_TESTS}" "${EXTERNAL_TESTS}")
endif()

unset(DEBUG_SCOPES CACHE)
