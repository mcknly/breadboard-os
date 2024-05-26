# PROJECT NAME - in quotes, no spaces
set(PROJ_NAME "my-bbos-proj")

# PROJECT VERSION - in quotes, no spaces, can contain alphanumeric if necessary
set(PROJ_VER "0.0")

# CLI INTERFACE - 0: use UART for CLI (default), 1: use USB for CLI
set(CLI_IFACE 0)

# BOARD - set the board being used (i.e. 'pico', 'pico_w', etc)
set(BOARD pico)

# ------------- Don't change anything below this line ------------

# BOARD
set(PICO_BOARD ${BOARD})

# Raspberry Pi Pico (RP2040) SDK
include(pico_sdk_import.cmake)

if(PICO_BOARD STREQUAL "pico_w")
    add_definitions(-DUSING_PICOW) # Pass this parameter to the preprocessor if board type is pico_w since some HW config needs to change
endif()

# initialize the Pico/RP2040 SDK
pico_sdk_init()

set(hardware_libs "pico_unique_id" "pico_stdlib")
set(hardware_includes hardware/rp2040)
set(hardware_subdir ${PROJECT_SOURCE_DIR}/hardware/rp2040)

