/******************************************************************************
 * @file services.c
 *
 * @brief Defines the service descriptors for each system service declared in
 *        services.h. These service descriptors are used to associate a service
 *        name string with a service function pointer, as well as indicate if
 *        the service should be started at boot. The individual service function
 *        implementations and FreeRTOS APIs for registering the associated tasks
 *        are contained in the source files for the respective services. Queues
 *        used to pass data between services are implemented in service_queues.h.
 *
 * @author @nbes4 (github)
 *
 * @date 06-18-2024
 *
 * @copyright Copyright (c) 2024 @nbes4 (github)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "services.h"

// see services.h for more information on creating services
// note: use xstr() to convert the SERVICE_NAME_ #define from services.h to a usable string
// (e.g. xstr(SERVICE_NAME_HEARTBEAT))
const service_desc_t service_descriptors[] = {
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
    },
    {.name = "wifi",
            .service_func = wifi_service,
            .startup = true
    },
};

const size_t service_descriptors_length = sizeof(service_descriptors)/sizeof(service_desc_t);
