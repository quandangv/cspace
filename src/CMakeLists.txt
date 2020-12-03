file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.c[p]*)
list(REMOVE_ITEM files main.cpp)

add_library(cs STATIC ${files})
target_include_directories(cs PUBLIC ${dirs})

add_executable(cscheme main.cpp)
target_link_libraries(cscheme cs)

install(TARGETS cscheme
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT runtime)

set(files ${files} PARENT_SCOPE)
