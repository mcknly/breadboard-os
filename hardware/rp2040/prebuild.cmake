set(PICO_BOARD ${BOARD})

# Raspberry Pi Pico (RP2040) SDK
include(${hardware_dir}/pico_sdk_import.cmake)

set(hardware_includes ${hardware_dir})
set(hardware_libs "pico_unique_id" "pico_stdlib")
