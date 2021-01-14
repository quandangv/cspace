# paths to various directories
get_filename_component(generated_headers_dir ${CMAKE_BINARY_DIR}/generated-headers ABSOLUTE)
get_filename_component(include_dir ${CMAKE_SOURCE_DIR}/include ABSOLUTE)
get_filename_component(private_headers_dir ${CMAKE_SOURCE_DIR}/private-headers ABSOLUTE)
get_filename_component(src_dir ${CMAKE_SOURCE_DIR}/src ABSOLUTE)
get_filename_component(test_dir ${CMAKE_SOURCE_DIR}/test ABSOLUTE)
set(header_dirs ${include_dir} ${private_headers_dir} ${generated_headers_dir})

# configure files {{{
  if(PLATFORM EQUAL "Linux")
    add_compile_definitions(PLATFORM_LINUX)
  endif()

  configure_file(${private_headers_dir}/common.hpp.in 
    ${generated_headers_dir}/common.hpp
    ESCAPE_QUOTES)

  unset(DEBUG_SCOPES CACHE)
# }}}

# public headers
set(HEADERS
  ${include_dir}/colorspace.hpp
  ${include_dir}/conversion.hpp
  ${include_dir}/error.hpp
  ${include_dir}/interface.hpp
  ${include_dir}/processor.hpp
)

# source files
set(SOURCES
  ${src_dir}/logger.cpp
  ${src_dir}/all_settings.cpp
  ${src_dir}/colorspace.cpp
  ${src_dir}/parse.cpp
  ${src_dir}/setting.cpp
  ${src_dir}/token_iterator.cpp
  ${src_dir}/tstring.cpp
  ${src_dir}/processor.cpp

  ${src_dir}/interface/add_term.cpp
  ${src_dir}/interface/general.cpp

  ${src_dir}/conversion/cielab.cpp
  ${src_dir}/conversion/cmyk.cpp
  ${src_dir}/conversion/cylindrical.cpp
  ${src_dir}/conversion/hsl_hsv.cpp
  ${src_dir}/conversion/jzazbz.cpp
  ${src_dir}/conversion/xyz.cpp
)

if(BUILD_TESTS)
  enable_testing()
  include(cmake/test)

  add_unit_test(parse)
  add_unit_test(interface)
  add_lib_test(conversion)
  add_lib_test(processor)
endif()
