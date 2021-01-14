if(PLATFORM EQUAL "Linux")
  add_compile_definitions(PLATFORM_LINUX)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/common.hpp.in 
  ${CMAKE_BINARY_DIR}/generated-headers/common.hpp
  ESCAPE_QUOTES)
