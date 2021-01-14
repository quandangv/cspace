# shared library {{{
# The shared library provides core functionalities
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
  target_include_directories(${PROJECT_NAME} PUBLIC ${header_dirs})

  set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")
  install(TARGETS ${PROJECT_NAME}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
# }}}

# executable {{{
# The executable depends on the shared library to provide a command-line interface.
  add_executable(${PROJECT_NAME}_exec ${src_dir}/main.cpp)
  target_link_libraries(${PROJECT_NAME}_exec PUBLIC ${PROJECT_NAME})

  set_target_properties(${PROJECT_NAME}_exec PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
  install(TARGETS ${PROJECT_NAME}_exec
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          COMPONENT runtime)
# }}}

set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_exec
  PROPERTIES VERSION ${PROJECT_VERSION}
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
