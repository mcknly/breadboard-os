cmake_minimum_required(VERSION 3.13)

### PROJECT SETTINGS ###

# PROJECT NAME - in quotes, no spaces
set(PROJ_NAME "my-bbos-proj")
# PROJECT VERSION - in quotes, no spaces, can contain alphanumeric if necessary
set(PROJ_VER "0.0")
# CLI INTERFACE - 0: use UART for CLI (default), 1: use USB for CLI
set(CLI_IFACE 0)
# BOARD TYPE - set the RP2040 "board" being used (i.e. 'pico', 'pico_w', etc)
set(PICO_BOARD pico)

### END PROJECT SETTINGS ###

# pull in setups for various modules
# Raspberry Pi Pico (RP2040) SDK
include(pico_sdk_import.cmake)
# FreeRTOS
include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# project definition
project(${PROJ_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DPROJECT_NAME=${PROJ_NAME})   # pass the project name to the preprocessor for use in the code
add_definitions(-DPROJECT_VERSION=${PROJ_VER}) # pass the project version number to the preprocessor for use in the code
add_definitions(-DBOARD_TYPE=${PICO_BOARD})    # pass the board type to the preprocessor for further decision making
add_definitions(-DCLI_USE_USB=${CLI_IFACE})    # 0: use UART for CLI (default), 1: use USB for CLI
#add_definitions(-DSCHED_TEST_DELAY) # uncomment to force delay tasks for scheduler testing, see 'task_sched_update()' function
if(PICO_BOARD STREQUAL "pico_w")
    add_definitions(-DUSING_PICOW) # Pass this parameter to the preprocessor if board type is pico_w since some HW config needs to change
endif()

# initialize the Pico/RP2040 SDK
pico_sdk_init()

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
    pico_stdlib
    hardware_i2c
    hardware_spi
    hardware_flash
    hardware_adc
    cmsis_core
    pico_unique_id
    tinyusb_device
)
target_include_directories(${PROJ_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/rtos
    ${PROJECT_SOURCE_DIR}/services
    ${PROJECT_SOURCE_DIR}/hardware/rp2040
    ${PROJECT_SOURCE_DIR}/driver_lib
    ${PROJECT_SOURCE_DIR}/littlefs/littlefs
    ${PROJECT_SOURCE_DIR}/git_version
)
add_subdirectory(${PROJECT_SOURCE_DIR}/rtos)
add_subdirectory(${PROJECT_SOURCE_DIR}/services)
add_subdirectory(${PROJECT_SOURCE_DIR}/hardware/rp2040)
add_subdirectory(${PROJECT_SOURCE_DIR}/driver_lib)

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

# disable Pico STDIO - interacting with CLI will be done via RTOS task queue only, no printf
pico_enable_stdio_usb(${PROJ_NAME} 0)
pico_enable_stdio_uart(${PROJ_NAME} 0)

# generate UF2, dissassembly and map files, etc
pico_add_extra_outputs(${PROJ_NAME})
