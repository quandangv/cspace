get_include_dirs(include_dirs)
get_sources_dir(src_dir)

# source tree {{{
  set(CSPACE_SOURCES
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
# }}}

# target: cspace {{{
# This is for the shared library that provides core functionalities
  add_library(cspace SHARED ${CSPACE_SOURCES})
  target_include_directories(cspace PUBLIC ${include_dirs})

  set_target_properties(cspace PROPERTIES VERSION ${PROJECT_VERSION})
  set_target_properties(cspace PROPERTIES PUBLIC_HEADER "${CSPACE_HEADERS}")
  install(TARGETS cspace
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cspace)
# }}}

# target: cspace_exec {{{
# This is for the executable, which depends on the shared library to provide a command-line interface.
  add_executable(cspace_exec main.cpp)
  target_link_libraries(cspace_exec PUBLIC cspace)

  set_target_properties(cspace_exec PROPERTIES VERSION ${PROJECT_VERSION})
  set_target_properties(cspace_exec PROPERTIES OUTPUT_NAME "cspace")
  install(TARGETS cspace_exec
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          COMPONENT runtime)
# }}}
