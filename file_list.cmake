# paths to various directories
get_filename_component(GENERATED_HEADERS_DIR ${CMAKE_BINARY_DIR}/generated-headers ABSOLUTE)
get_filename_component(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include ABSOLUTE)
get_filename_component(PRIVATE_HEADERS_DIR ${CMAKE_SOURCE_DIR}/private-headers ABSOLUTE)
get_filename_component(SRC_DIR ${CMAKE_SOURCE_DIR}/src ABSOLUTE)
get_filename_component(TEST_DIR ${CMAKE_SOURCE_DIR}/test ABSOLUTE)
set(HEADER_DIRS ${INCLUDE_DIR} ${PRIVATE_HEADERS_DIR} ${GENERATED_HEADERS_DIR})

# configure files {{{
  if(PLATFORM EQUAL "Linux")
    add_compile_definitions(PLATFORM_LINUX)
  endif()

  configure_file(${PRIVATE_HEADERS_DIR}/common.hpp.in 
    ${GENERATED_HEADERS_DIR}/common.hpp
    ESCAPE_QUOTES)

  unset(DEBUG_SCOPES CACHE)
# }}}

# public headers
set(HEADERS
  ${INCLUDE_DIR}/colorspace.hpp
  ${INCLUDE_DIR}/conversion.hpp
  ${INCLUDE_DIR}/error.hpp
  ${INCLUDE_DIR}/interface.hpp
  ${INCLUDE_DIR}/processor.hpp
)

# source files
set(SOURCES
  ${SRC_DIR}/logger.cpp
  ${SRC_DIR}/all_settings.cpp
  ${SRC_DIR}/colorspace.cpp
  ${SRC_DIR}/parse.cpp
  ${SRC_DIR}/setting.cpp
  ${SRC_DIR}/token_iterator.cpp
  ${SRC_DIR}/tstring.cpp
  ${SRC_DIR}/processor.cpp

  ${SRC_DIR}/interface/add_term.cpp
  ${SRC_DIR}/interface/general.cpp

  ${SRC_DIR}/conversion/cielab.cpp
  ${SRC_DIR}/conversion/cmyk.cpp
  ${SRC_DIR}/conversion/cylindrical.cpp
  ${SRC_DIR}/conversion/hsl_hsv.cpp
  ${SRC_DIR}/conversion/jzazbz.cpp
  ${SRC_DIR}/conversion/xyz.cpp
)

set(INTERNAL_TESTS parse interface)
set(EXTERNAL_TESTS conversion processor)
