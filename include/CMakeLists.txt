get_include_dirs(include_dir)

# public headers {{{
  set(CSPACE_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/colorspace.hpp
    ${CMAKE_CURRENT_LIST_DIR}/conversion.hpp
    ${CMAKE_CURRENT_LIST_DIR}/error.hpp
    ${CMAKE_CURRENT_LIST_DIR}/interface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/processor.hpp
  PARENT_SCOPE)
# }}}
