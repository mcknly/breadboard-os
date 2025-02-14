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
    add_compile_definitions(HAS_CYW43)
    if(ENABLE_WIFI)
        # include CY43 lwIP support
        list(APPEND hardware_libs   #"pico_cyw43_arch_lwip_sys_freertos"         # for lwip NO_SYS=0
                                    "pico_cyw43_arch_lwip_threadsafe_background" # for lwip NO_SYS=1
        )
        list(APPEND hardware_includes ${hardware_dir}/net_inc)
        if(ENABLE_HTTPD)
            list(APPEND hardware_libs   "lwip_httpd_content"
                                        "pico_lwip_mdns"
                                        "pico_lwip_http"
            )
            list(APPEND hardware_includes ${PICO_LWIP_CONTRIB_PATH}/apps/httpd)
        endif()
    else()
        list(APPEND hardware_libs pico_cyw43_arch_none) # basic CYW43 support
        set(ENABLE_WIFI false)
    endif()
else()
    if(ENABLE_WIFI)
        message(WARNING "Board does not support WiFi, disabling this option")
        set(ENABLE_WIFI false)
    endif()
endif()
# Make sure no networking features are enabled if WiFi is disabled
if(NOT ENABLE_WIFI AND ENABLE_HTTPD)
    message(WARNING "Disabling httpd due to WiFi option being disabled")
    set(ENABLE_HTTPD false)
endif()
