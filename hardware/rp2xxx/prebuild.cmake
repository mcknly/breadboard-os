# PICO_BOARD should be one of the boards listed in Pico SDK header files
# see: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
# Set BOARD in project.cmake accordingly (i.e. pico, pico_w, pico2, etc)
set(PICO_BOARD ${BOARD})

# PICO_PLATFORM is not necessary for the SDK as long as PICO_BOARD is set,
# however Rasberry Pi's FreeRTOS port uses it for kernel import
#set(PICO_PLATFORM "rp2350")

# Raspberry Pi Pico (RP2040/RP2350) SDK
include(${hardware_dir}/pico_sdk_import.cmake)

# Platform-specific variable for paths/libraries/etc
set(hardware_includes ${hardware_dir})
set(hardware_libs "pico_unique_id" "pico_stdlib")

# FreeRTOS port subdirectory for this platform (relative to $FREERTOS_KERNEL_PATH)
#set(freertos_port_path "portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake")
set(freertos_port_path "portable/ThirdParty/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake")

# If using a Pico wireless variant, include the cyw43 library
if(PICO_BOARD STREQUAL "pico_w")
    list(APPEND hardware_libs "pico_cyw43_arch_none")
endif()
