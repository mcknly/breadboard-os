cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Project/hardware-specific setup - set platform in project.cmake
include(project.cmake)
set(hardware_dir ${CMAKE_CURRENT_SOURCE_DIR}/hardware/${PLATFORM})
if(EXISTS ${hardware_dir}/prebuild.cmake)
    include(${hardware_dir}/prebuild.cmake)
endif()
project(${PROJ_NAME} C CXX ASM)

add_definitions(-DPROJECT_NAME=${PROJ_NAME})   # pass the project name to the preprocessor for use in the code
add_definitions(-DPROJECT_VERSION=${PROJ_VER}) # pass the project version number to the preprocessor for use in the code
add_definitions(-DCLI_USE_USB=${CLI_IFACE})    # 0: use UART for CLI (default), 1: use USB for CLI
add_definitions(-DBOARD=${BOARD})              # pass the board type to the preprocessor for further decision making
#add_definitions(-DSCHED_TEST_DELAY)           # uncomment to force delay tasks for scheduler testing, see 'task_sched_update()' function

# FreeRTOS
include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# add cli/microshell library subdirectory (submodule)
add_subdirectory(${PROJECT_SOURCE_DIR}/cli cli)

# add littlefs library subdirectory (submodule)
add_subdirectory(${PROJECT_SOURCE_DIR}/littlefs littlefs)

# add git version library subdirectory (submodule)
add_subdirectory(${PROJECT_SOURCE_DIR}/git_version cmake_git_version_tracking)

# create the main executable
add_executable(${PROJ_NAME}
    main.c
)
target_link_libraries(${PROJ_NAME} PUBLIC
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    cli
    littlefs
    cmake_git_version_tracking
    hardware_i2c
    hardware_spi
    hardware_flash
    hardware_adc
    cmsis_core
    tinyusb_device
    ${hardware_libs}
)
target_include_directories(${PROJ_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/rtos
    ${PROJECT_SOURCE_DIR}/services
    ${PROJECT_SOURCE_DIR}/driver_lib
    ${PROJECT_SOURCE_DIR}/littlefs/littlefs
    ${PROJECT_SOURCE_DIR}/git_version
    ${PROJECT_SOURCE_DIR}/${hardware_includes}
)

add_subdirectory(${PROJECT_SOURCE_DIR}/rtos)
add_subdirectory(${PROJECT_SOURCE_DIR}/services)
add_subdirectory(${PROJECT_SOURCE_DIR}/driver_lib)
if(DEFINED hardware_dir)
    add_subdirectory(${hardware_dir})
endif()

# add any additional definitions, options
target_compile_definitions(${PROJ_NAME} PUBLIC 
    CFG_TUSB_CONFIG_FILE="hardware_config.h" # override standard TinyUSB config file location
)
target_compile_options(
    ${PROJ_NAME}
    PRIVATE
    -Werror -g -O0
)
# TODO: try to add -Wall and -Wextra to compile options to clean up more warnings

# Any pre-build stuff the hardware module needs, as a function
if(COMMAND hardware_build_extra)
    cmake_language(CALL hardware_build_extra)
endif()
