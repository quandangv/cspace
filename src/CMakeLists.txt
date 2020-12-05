file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.c[p]*)
list(REMOVE_ITEM files main.cpp)

add_library(space STATIC ${files})
target_include_directories(space PUBLIC ${dirs})

add_executable(cspace main.cpp)
target_link_libraries(cspace space)

install(TARGETS cspace
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT runtime)

set(files ${files} PARENT_SCOPE)
