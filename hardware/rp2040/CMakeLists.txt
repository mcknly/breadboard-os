add_library(board_hal INTERFACE)

# add source files to the top-level project
target_sources(board_hal INTERFACE
    hardware_config.c
    hw_gpio.c
    hw_uart.c
    hw_i2c.c
    hw_spi.c
    hw_adc.c
    hw_usb.c
    onboard_led.c
    onboard_flash.c
    hw_watchdog.c
    hw_reset.c
    hw_clocks.c
    hw_cores.c
    hw_versions.c
)

target_include_directories(board_hal INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(board_hal INTERFACE
    pico_stdlib
    pico_unique_id
    hardware_i2c
    hardware_spi
    hardware_flash
    hardware_adc
    tinyusb_device
)

target_compile_definitions(board_hal INTERFACE CFG_TUSB_CONFIG_FILE="hardware_config.h")

if(PICO_BOARD STREQUAL pico_w)
    # Pass this parameter to the preprocessor if board type is pico_w
    # since some HW config needs to change
    target_compile_definitions(board_hal INTERFACE -DUSING_PICOW)
    target_link_libraries(board_hal INTERFACE pico_cyw43_arch_none)
endif()

function(hardware_build_extra)
    # initialize the Pico/RP2040 SDK
    pico_sdk_init()

    # disable Pico STDIO - interacting with CLI will be done via RTOS task queue only, no printf
    pico_enable_stdio_usb(${PROJ_NAME} 0)
    pico_enable_stdio_uart(${PROJ_NAME} 0)

    # generate UF2, dissassembly and map files, etc
    pico_add_extra_outputs(${PROJ_NAME})
endfunction()
