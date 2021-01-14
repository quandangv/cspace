# Download and unpack googletest at configure time {{{
  configure_file(${CMAKE_CURRENT_LIST_DIR}/googletest.in
                 ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                  RESULT_VARIABLE result
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()

  execute_process(COMMAND ${CMAKE_COMMAND} --build .
                  RESULT_VARIABLE result
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Add googletest directly to our build. This defines
  # the gtest, gtest_main, gmock and gmock_main targets.
  add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                   ${CMAKE_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)
# }}}

add_custom_target(${PROJECT_NAME}_tests COMMENT "Building all unit test")
add_library(lib${PROJECT_NAME} SHARED IMPORTED)
set_property(TARGET lib${PROJECT_NAME} PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/bin/lib${PROJECT_NAME}.so)

# Utility functions to add unit tests {{{
  function(setup_unit_test source_file output_name)
    string(REPLACE "/" "_" testname ${source_file})
    set(name "test.${testname}")
    set(${output_name} ${name} PARENT_SCOPE)
    add_executable(${name} ${test_dir}/${source_file}.cpp)
    add_test(NAME ${name} COMMAND ${name})
    add_dependencies(${PROJECT_NAME}_tests ${name})
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
  endfunction()

  # Add internal unit tests. Public and private headers are visible to them
  function(add_unit_test source_file)
    setup_unit_test(${source_file} name)
    # Link against gmock (this automatically links against gtest)
    target_link_libraries(${name} ${PROJECT_NAME} gmock_main)
  endfunction()

  # Add external unit tests. Only the public headers are visible to them
  function(add_lib_test source_file)
    setup_unit_test(${source_file} name)
    target_link_libraries(${name} lib${PROJECT_NAME} gmock_main)
    target_include_directories(${name} PUBLIC ${PROJECT_SOURCE_DIR}/include ${private_headers_dir})
  endfunction()
# }}}

# Run make check to build and run all unit tests
add_custom_target(check COMMAND GTEST_COLOR=1 ctest --output-on-failure
                        DEPENDS ${PROJECT_NAME}_tests)
