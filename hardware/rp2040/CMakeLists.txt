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
    # disable Pico STDIO - interacting with CLI will be done via RTOS task queue only, no printf
    pico_enable_stdio_usb(${PROJ_NAME} 0)
    pico_enable_stdio_uart(${PROJ_NAME} 0)

    # generate UF2, dissassembly and map files, etc
    pico_add_extra_outputs(${PROJ_NAME})
endfunction()
