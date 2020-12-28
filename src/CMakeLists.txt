file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.c[p]*)
list(REMOVE_ITEM files main.cpp)

# target: cspace {{{

add_library(cspace SHARED ${files})

target_include_directories(cspace PUBLIC ${dirs})
set_target_properties(cspace PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cspace PROPERTIES PUBLIC_HEADER "${public_headers}")

install(TARGETS cspace
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cspace)

# }}}

# target: cspace_exec {{{

add_executable(cspace_exec main.cpp)
target_link_libraries(cspace_exec PUBLIC cspace)
set_target_properties(cspace_exec PROPERTIES OUTPUT_NAME "cspace")

install(TARGETS cspace_exec
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT runtime)

# }}}

set(files ${files} PARENT_SCOPE)
