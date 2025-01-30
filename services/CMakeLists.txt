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

if (ENABLE_WIFI)
    target_sources(${PROJ_NAME} PRIVATE
        netman_service.c
    )
endif()
