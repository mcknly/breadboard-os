# PICO_BOARD should be one of the boards listed in Pico SDK header files
# see: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
# Set BOARD in project.cmake accordingly (i.e. pico, pico_w, pico2, etc)
set(PICO_BOARD ${BOARD})

# Raspberry Pi Pico (RP2040/RP2350) SDK
include(${hardware_dir}/pico_sdk_import.cmake)

# Platform-specific variable for paths/libraries/etc
set(hardware_includes ${hardware_dir})
set(hardware_libs   "pico_unique_id"
                    "pico_stdlib"
                    "hardware_i2c"
                    "hardware_spi"
                    "hardware_flash"
                    "hardware_adc"
                    "cmsis_core"
                    "tinyusb_device"                    
)

# FreeRTOS port subdirectory for this platform (relative to $FREERTOS_KERNEL_PATH)
# all of the "boards" from the link above should be handled here to categorize
# them as RP2040 or RP2350 type
if(PICO_BOARD STREQUAL "pico" OR PICO_BOARD STREQUAL "pico_w")
    set(freertos_port_path "portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake")
elseif(PICO_BOARD STREQUAL "pico2" OR PICO_BOARD STREQUAL "pico2_w")
    set(freertos_port_path "portable/ThirdParty/Community-Supported-Ports/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake")
else()
    message(FATAL_ERROR "Board type unknown to BBOS: ${PICO_BOARD} - see hardware/rp2xxx/prebuild.cmake for instructions")
endif()

# If using a Pico wireless variant, include the cyw43 library
# Any other board names that use cyw43 would have to be added here too
if(PICO_BOARD STREQUAL "pico_w" OR PICO_BOARD STREQUAL "pico2_w")
    list(APPEND hardware_libs pico_cyw43_arch_none)
    add_compile_definitions(HAS_CYW43)
endif()
