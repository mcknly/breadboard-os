#include "services.h"
 
 const ServiceDesc_t service_descriptors[] = {
    {
        .name = xstr(SERVICE_NAME_USB), 
        .service_func = usb_service,
        .startup = true
    },
    {
        .name = xstr(SERVICE_NAME_CLI), 
        .service_func = cli_service,
        .startup = true
    }, 
    {
        .name = xstr(SERVICE_NAME_STORMAN), 
        .service_func = storman_service,
        .startup = true
    }, 
    {
        .name = xstr(SERVICE_NAME_WATCHDOG), 
        .service_func = watchdog_service,
        .startup = true
    }, 
    {
        .name = xstr(SERVICE_NAME_HEARTBEAT), 
        .service_func = heartbeat_service,
        .startup = false
    }
};

const int service_descriptors_length = sizeof(service_descriptors)/sizeof(ServiceDesc_t);