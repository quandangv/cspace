include_directories(${CMAKE_CURRENT_LIST_DIR})

# Download and unpack googletest at configure time {{{
configure_file(
  CMakeLists.txt.in
  ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt
)
execute_process(
  COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
)

if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
)

if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Add googletest directly to our build. This defines
# the gtest, gtest_main, gmock and gmock_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# }}}

# Compile all unit tests with 'make cspace_tests'
add_custom_target(cspace_tests COMMENT "Building all unit test")
add_custom_target(cspace_libtests COMMENT "Building all library unit test")

function(add_unit_test source_file)
  string(REPLACE "/" "_" testname ${source_file})
  set(name "unit_test.${testname}")

  add_executable(${name} ${source_file}.cpp)

  # Link against gmock (this automatically links against gtest)
  target_link_libraries(${name} cspace gmock_main)
  add_test(NAME ${name} COMMAND ${name})

  add_dependencies(cspace_tests ${name})
endfunction()

add_library(libcspace SHARED IMPORTED)
set_property(TARGET libcspace PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/src/libcspace.so)
function(add_lib_test source_file)
  string(REPLACE "/" "_" testname ${source_file})
  set(name "unit_test.${testname}")

  add_executable(${name} ${source_file}.cpp)

  # Link against gmock (this automatically links against gtest)
  target_link_libraries(${name} libcspace gmock_main)
  target_include_directories(${name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
  add_test(NAME ${name} COMMAND ${name})

  add_dependencies(cspace_libtests ${name})
endfunction()

add_unit_test(parse)
add_lib_test(conversion)
add_unit_test(interface)
add_lib_test(processor)

# Run make check to build and run all unit tests
add_custom_target(check
                  COMMAND GTEST_COLOR=1 ctest --output-on-failure
                  DEPENDS cspace_tests cspace_libtests)

