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
    hw_registers.c
    hw_reset.c
    hw_clocks.c
    hw_cores.c
    hw_versions.c
)

# optional features
if (ENABLE_WIFI)
    target_sources(${PROJ_NAME} PRIVATE
        hw_wifi.c
        hw_net.c
    )
endif()

function(hardware_build_extra)
    # initialize the Pico/RP2040 SDK
    pico_sdk_init()

    # Pass these parameters to the preprocessor if using specific board/chip types
    # since some HW config needs to change
    if(PICO_PLATFORM STREQUAL "rp2040") # the Pico SDK import will auto set PICO_PLATFORM based on PICO_BOARD
        target_compile_definitions(${PROJ_NAME} PUBLIC -DUSING_RP2040)
        target_compile_definitions(cli PUBLIC -DUSING_RP2040)
    endif()
    if(PICO_PLATFORM STREQUAL "rp2350-arm-s") # the Pico SDK import will auto set PICO_PLATFORM based on PICO_BOARD
        target_compile_definitions(${PROJ_NAME} PUBLIC -DUSING_RP2350)
        target_compile_definitions(cli PUBLIC -DUSING_RP2350)
    endif()

    # Pass optional build options to the preprocessor for various code inclusions
    if(ENABLE_WIFI)
        target_compile_definitions(${PROJ_NAME} PUBLIC -DHW_USE_WIFI
                                                       -DCYW43_HOST_NAME=\"${HOSTNAME}-${BOARD}\")
        target_compile_definitions(cli PUBLIC -DHW_USE_WIFI
                                              -DHOST_NAME=\"${HOSTNAME}\")
        if(ENABLE_HTTPD)
            # defines used for code inclusion
            target_compile_definitions(${PROJ_NAME} PUBLIC -DENABLE_HTTPD)
            target_compile_definitions(cli PUBLIC -DENABLE_HTTPD)
            # pico-sdk function for building http content into a library
            pico_add_library(lwip_httpd_content)
            pico_set_lwip_httpd_content(lwip_httpd_content INTERFACE
            ${PROJECT_SOURCE_DIR}/hardware/rp2xxx/net_inc/httpd_content/404.html
            ${PROJECT_SOURCE_DIR}/hardware/rp2xxx/net_inc/httpd_content/index.shtml
            ${PROJECT_SOURCE_DIR}/hardware/rp2xxx/net_inc/httpd_content/test.shtml
            )
        endif()
    endif()

    # disable Pico STDIO - interacting with CLI will be done via RTOS task queue only, no printf
    pico_enable_stdio_usb(${PROJ_NAME} 0)
    pico_enable_stdio_uart(${PROJ_NAME} 0)

    # generate UF2, dissassembly and map files, etc
    pico_add_extra_outputs(${PROJ_NAME})
endfunction()
