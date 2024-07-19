# add source files to the top-level project
target_sources(${PROJ_NAME} PRIVATE
    services.c
    service_queues.c
    cli_service.c
    usb_service.c
    taskman_service.c
    storman_service.c
    watchdog_service.c
    heartbeat_service.c
)

if (DEFINED HW_WIFI AND HW_WIFI)
    target_sources(${PROJ_NAME} PRIVATE wifi_service.c)
    if(NOT DEFINED ENV{WIFI_SSID} OR NOT DEFINED ENV{WIFI_PASSWORD})
        message(FATAL_ERROR "must define WIFI_SSID and WIFI_PASSWORD environment variables to use wifi")
    endif()
    target_compile_definitions(${PROJ_NAME} PRIVATE
        WIFI_SSID=\"$ENV{WIFI_SSID}\"
        WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
    )
endif()
