list(APPEND dirs ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_BINARY_DIR}/generated_src)
configure_file(${CMAKE_CURRENT_LIST_DIR}/common.hpp.in 
               ${CMAKE_BINARY_DIR}/generated_src/common.hpp
               ESCAPE_QUOTES)

list(APPEND public_headers ${CMAKE_BINARY_DIR}/generated_src/option.hpp)

set(dirs ${dirs} PARENT_SCOPE)
set(public_headers ${public_headers} PARENT_SCOPE)
