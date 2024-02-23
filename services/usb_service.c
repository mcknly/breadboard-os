/******************************************************************************
 * @file usb_service.c
 *
 * @brief USB service implementation and FreeRTOS task creation.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 02-14-2024
 * 
 * @copyright Copyright (c) 2024 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 * 
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "hardware_config.h"
#include "rtos_utils.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "tusb.h"
#include "string.h"


static void prvUsbTask(void *pvParameters); // usb task
TaskHandle_t xUsbTask;

// main service function, creates FreeRTOS task from prvUsbTask
BaseType_t usb_service(void)
{
    BaseType_t xReturn;

    // spawn the usb task
    xReturn = xTaskCreate(
        prvUsbTask,
        xstr(SERVICE_NAME_USB),
        STACK_USB,
        NULL,
        PRIORITY_USB,
        &xUsbTask
    );

    // print timestamp value
    cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        cli_uart_puts("USB service started\r\n");
    }
    else {
        cli_uart_puts("Error starting the USB service\r\n");
    }

    return xReturn;
}

// FreeRTOS task created by storman_service
static void prvUsbTask(void *pvParameters)
{
    // USB device endpoint 0
    struct usb_iface_t usb_iface_0 = {
        .iface_id = 0,
        .is_conn = false,
        .rx_buffer = {},
        .rx_pos = 0,
        .tx_buffer = {},
        .tx_pos = 0
    };

    while(true) {
        // TinyUSB service function
        tud_task();

        // perform USB reads/writes and move data buffers to/from queue,
        // only if not using CLI over USB. If using USB for CLI, run tud_task only
        if (!CLI_USE_USB) {
            // if connected, read/write any data available in buffers
            if (tud_cdc_n_connected(usb_iface_0.iface_id)) {
                usb_iface_0.is_conn = true;
                usb_read_bytes(&usb_iface_0);
                usb_write_bytes(&usb_iface_0);
            }
            else usb_iface_0.is_conn = false;

            // check if there are RX bytes in the buffer and copy into the receive queue
            if (usb_iface_0.rx_pos > 0) {
                // add null termination to buffer if needed
                if (usb_iface_0.rx_buffer[usb_iface_0.rx_pos - 1] != '\0') { // if it is a string there should already be NULL in pos-1
                    usb_iface_0.rx_buffer[usb_iface_0.rx_pos] = '\0';        // if not, add one
                }
                if (xQueueSend(usb0_rx_queue, &usb_iface_0.rx_buffer, 10) == pdTRUE) {
                    usb_iface_0.rx_pos = 0;
                }
            }

            // check if there are any TX items in the queue and copy to the TX data buffer
            if (usb_iface_0.tx_pos == 0) {
                if (xQueueReceive(usb0_tx_queue, usb_iface_0.tx_buffer, 0) == pdTRUE) {
                    usb_iface_0.tx_pos = strlen(usb_iface_0.tx_buffer) + 1; // add 1 assuming null terminated byte array
                }
            }
        }

        // update this task's schedule
        task_sched_update(REPEAT_USB, DELAY_USB);
    }
}