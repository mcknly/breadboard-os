# add source files to the top-level project
target_sources(${PROJ_NAME} PRIVATE
    services.c
    service_queues.c
    cli_service.c
    usb_service.c
    taskman_service.c
    storman_service.c
    netman_service.c
    watchdog_service.c
    heartbeat_service.c
)

# add wifi ssid/password from environment variables for networkmanager service
target_compile_definitions(${PROJ_NAME} PRIVATE
    WIFI_SSID="$ENV{WIFI_SSID}"
    WIFI_PWD="$ENV{WIFI_PWD}"
)