set(PICO_BOARD ${BOARD})

# Raspberry Pi Pico (RP2040) SDK
include(${hardware_dir}/pico_sdk_import.cmake)

set(hardware_includes ${hardware_dir})
set(hardware_libs "pico_unique_id" "pico_stdlib")

if(PICO_BOARD STREQUAL "pico_w")
    list(APPEND hardware_libs "pico_cyw43_arch_none")
endif()
