cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)

project(cspace VERSION 1.1.0)

list(TRANSFORM CMAKE_MODULE_PATH PREPEND ${CMAKE_CURRENT_LIST_DIR})
include(file_list.cmake)
include(cmake/targets.cmake)
add_shared_lib(cspace "${SOURCES}" "${INCLUDE_DIRS}" "${HEADERS}" "cspace")

configure_file(${PRIVATE_HEADERS_DIR}/common.hpp.in  ${GENERATED_HEADERS_DIR}/common.hpp)

if(BUILD_TESTS)
  include(cmake/test.cmake)
endif()
