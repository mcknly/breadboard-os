# This file configures the build for the microshell submodule, which is included
# as source for the CLI which will be built as a static library to be used by
# the main application.

# pull in microshell setup
add_subdirectory(${PROJECT_SOURCE_DIR}/microshell/src microshell)

# run microshell build setup script
build_ush(microshell)

# defines to use in microshell build
target_compile_definitions(
    microshell
    PUBLIC
    PICO
)

# create cli library to use at top level (linked against microshell)
add_library(cli STATIC
    ${CMAKE_CURRENT_LIST_DIR}/shell.c
    ${CMAKE_CURRENT_LIST_DIR}/shell_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/cli_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/node_root.c
    ${CMAKE_CURRENT_LIST_DIR}/node_dev.c
    ${CMAKE_CURRENT_LIST_DIR}/node_bin.c
    ${CMAKE_CURRENT_LIST_DIR}/node_proc.c
    ${CMAKE_CURRENT_LIST_DIR}/node_mnt.c
    ${CMAKE_CURRENT_LIST_DIR}/node_etc.c
    ${CMAKE_CURRENT_LIST_DIR}/node_lib.c
    ${CMAKE_CURRENT_LIST_DIR}/node_net.c
)
target_link_libraries(cli PUBLIC
    microshell
    pico_stdlib
    FreeRTOS-Kernel
    cmake_git_version_tracking
)
target_include_directories(cli PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/rtos
    ${PROJECT_SOURCE_DIR}/driver_lib
    ${PROJECT_SOURCE_DIR}/services
    ${PROJECT_SOURCE_DIR}/littlefs/littlefs
    ${PROJECT_SOURCE_DIR}/git_version
    ${hardware_dir}
)
target_compile_options(
    cli
    PRIVATE
    -Werror -g -O0
)
# TODO: try to add -Wall and -Wextra to compile options to clean up more warnings

# additional build options
if(ENABLE_MOTD)
        target_compile_definitions(cli PUBLIC -DPRINT_MOTD_AT_BOOT)
endif()