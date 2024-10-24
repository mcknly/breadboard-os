# PICO_BOARD should be one of the boards listed in Pico SDK header files
# see: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
# Set BOARD in project.cmake accordingly (i.e. pico, pico_w, pico2, etc)
set(PICO_BOARD ${BOARD})

# Raspberry Pi Pico (RP2040/RP2350) SDK
include(${hardware_dir}/pico_sdk_import.cmake)

# Platform-specific variable for paths/libraries/etc
set(hardware_includes ${hardware_dir})
set(hardware_libs "pico_unique_id" "pico_stdlib")

# FreeRTOS port subdirectory for this platform (relative to $FREERTOS_KERNEL_PATH)
# If it is a RP2350 board the board name needs to be added here to select the right port
if(PICO_BOARD STREQUAL "pico2")
    set(freertos_port_path "portable/ThirdParty/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake")
else()
    set(freertos_port_path "portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake")
endif()

# If using a Pico wireless variant, include the cyw43 library
# Any other board names that use cyw43 would have to be added here too
if(PICO_BOARD STREQUAL "pico_w")
    list(APPEND hardware_libs pico_cyw43_arch_none)
    add_compile_definitions(USING_CYW43)
endif()
