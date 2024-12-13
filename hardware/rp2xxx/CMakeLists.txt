# add source files to the top-level project
target_sources(${PROJ_NAME} PRIVATE
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

function(hardware_build_extra)
    # initialize the Pico/RP2040 SDK
    pico_sdk_init()

    # Pass these parameters to the preprocessor if using specific board/chip types
    # since some HW config needs to change
    if(PICO_BOARD STREQUAL "pico_w")
        target_compile_definitions(cli PUBLIC -DUSING_PICOW)
    endif()
    if(PICO_PLATFORM STREQUAL "rp2350-arm-s") # the Pico SDK import will auto set this based on PICO_BOARD
        target_compile_definitions(${PROJ_NAME} PUBLIC -DUSING_RP2350)
        target_compile_definitions(cli PUBLIC -DUSING_RP2350)
    endif()

    # disable Pico STDIO - interacting with CLI will be done via RTOS task queue only, no printf
    pico_enable_stdio_usb(${PROJ_NAME} 0)
    pico_enable_stdio_uart(${PROJ_NAME} 0)

    # generate UF2, dissassembly and map files, etc
    pico_add_extra_outputs(${PROJ_NAME})
endfunction()
