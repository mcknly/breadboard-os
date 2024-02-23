# add source files to the top-level project
target_sources(${PROJ_NAME} PRIVATE
    device_drivers.c
    bme280.c
    mcp4725.c
)