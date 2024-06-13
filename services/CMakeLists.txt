# add source files to the top-level project
target_sources(${PROJ_NAME} PRIVATE
    service_queues.c
    cli_service.c
    usb_service.c
    taskman_service.c
    storman_service.c
    watchdog_service.c
    heartbeat_service.c
    services.c
)