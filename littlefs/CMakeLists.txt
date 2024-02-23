# This file configures the build for the littlefs submodule, which is built
# as a static library to be used by the main application.
# The littlefs submodule is nested within another littlefs directory for ease
# of wrapping in CMake (littlefs project is Makefile-based)

# create littlefs library to use at top level
add_library(littlefs STATIC
    ${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs_util.c
    ${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs.c
)
target_include_directories(littlefs PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/littlefs
)
target_compile_options(
    littlefs
    PRIVATE
    -Werror -g -O0
)
# TODO: try to add -Wall and -Wextra to compile options to clean up more warnings