# Load strings submodule
if(NOT DEFINED STRINGS_PUBLIC_HEADERS_DIR)
  execute_process(COMMAND git submodule update --init --rebase -- ${CMAKE_CURRENT_LIST_DIR}/lib/strings)
  include(lib/strings/file_list)
  set(STRINGS_PUBLIC_HEADERS_DIR ${PUBLIC_HEADERS_DIR})
endif()
set(INCLUDE_DIRS ${STRINGS_PUBLIC_HEADERS_DIR})

# paths to various directories
get_filename_component(GENERATED_HEADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated-headers ABSOLUTE)
get_filename_component(PUBLIC_HEADERS_DIR    ${CMAKE_CURRENT_LIST_DIR}/include ABSOLUTE)
get_filename_component(PRIVATE_HEADERS_DIR   ${CMAKE_CURRENT_LIST_DIR}/private-headers ABSOLUTE)
get_filename_component(SRC_DIR               ${CMAKE_CURRENT_LIST_DIR}/src ABSOLUTE)
get_filename_component(TEST_DIR              ${CMAKE_CURRENT_LIST_DIR}/test ABSOLUTE)
get_filename_component(LIBRARY_DIR           ${CMAKE_CURRENT_LIST_DIR}/lib ABSOLUTE)
list(APPEND INCLUDE_DIRS ${PUBLIC_HEADERS_DIR} ${PRIVATE_HEADERS_DIR} ${GENERATED_HEADERS_DIR})

unset(DEBUG_SCOPES CACHE)

# public headers
set(HEADERS
  ${PUBLIC_HEADERS_DIR}/colorspace.hpp
  ${PUBLIC_HEADERS_DIR}/conversion.hpp
  ${PUBLIC_HEADERS_DIR}/error.hpp
  ${PUBLIC_HEADERS_DIR}/interface.hpp
  ${PUBLIC_HEADERS_DIR}/processor.hpp
)

# source files
set(SOURCES
  ${SRC_DIR}/logger.cpp
  ${SRC_DIR}/all_settings.cpp
  ${SRC_DIR}/colorspace.cpp
  ${SRC_DIR}/parse.cpp
  ${SRC_DIR}/setting.cpp
  ${SRC_DIR}/processor.cpp

  ${SRC_DIR}/interface/add_term.cpp
  ${SRC_DIR}/interface/general.cpp

  ${SRC_DIR}/conversion/cielab.cpp
  ${SRC_DIR}/conversion/cmyk.cpp
  ${SRC_DIR}/conversion/cylindrical.cpp
  ${SRC_DIR}/conversion/hsl_hsv.cpp
  ${SRC_DIR}/conversion/jzazbz.cpp
  ${SRC_DIR}/conversion/xyz.cpp
  ${TSTRING_SOURCES}
)

set(INTERNAL_TESTS parse interface)
set(EXTERNAL_TESTS conversion processor)
set(TEST_FILES)
