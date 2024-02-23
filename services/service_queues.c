/******************************************************************************
 * @file service_queues.c
 *
 * @brief Initialization and helper functions for interaction with the queues
 *        used by FreeRTOS tasks.
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

#include <string.h>
#include "services.h"
#include "service_queues.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "queue.h"

// global declarations in services.h
QueueHandle_t print_queue;
QueueHandle_t taskman_queue;
QueueHandle_t storman_queue;
QueueHandle_t usb0_rx_queue;
QueueHandle_t usb0_tx_queue;

// create task queues
bool init_queues(void) {
    // initialize all queues
    print_queue = xQueueCreate(PRINT_QUEUE_DEPTH, PRINT_QUEUE_ITEM_SIZE);
    taskman_queue = xQueueCreate(TASKMAN_QUEUE_DEPTH, TASKMAN_QUEUE_ITEM_SIZE);
    storman_queue = xQueueCreate(STORMAN_QUEUE_DEPTH, STORMAN_QUEUE_ITEM_SIZE);
    usb0_rx_queue = xQueueCreate(USB0_RX_QUEUE_DEPTH, USB0_RX_QUEUE_ITEM_SIZE);
    usb0_tx_queue = xQueueCreate(USB0_TX_QUEUE_DEPTH, USB0_TX_QUEUE_ITEM_SIZE);

    // make sure they were all created successfully
    if (print_queue   != NULL &&
        taskman_queue != NULL &&
        storman_queue != NULL &&
        usb0_rx_queue != NULL &&
        usb0_tx_queue != NULL) {
        return 0;
    } else {
        return 1;
    }
}


/*******************************************
 * helper functions to interact with queues
********************************************/

bool cli_print_raw(char *string) {
    if (xQueueSend(print_queue, string, 10) == pdTRUE) { // add string to print queue, waiting 10 os ticks max
        return true;
    }
    else return false;
}

bool cli_print_timestamped(char *string) {
    bool queue_post_result;
    char *timestamped_string = pvPortMalloc(TIMESTAMP_LEN + strlen(string));
    sprintf(timestamped_string, timestamp());
    sprintf(timestamped_string + strlen(timestamped_string), string);
    if (xQueueSend(print_queue, timestamped_string, 10) == pdTRUE) { // add string to print queue, waiting 10 os ticks max
        queue_post_result = true;
    }
    else queue_post_result = false;
    vPortFree(timestamped_string);
    return queue_post_result;
}

bool taskman_request(struct taskman_item_t *tmi) {
    if (xQueueSend(taskman_queue, tmi, 10) == pdTRUE) { // add request item to taskmanager queue, waiting 10 os ticks max
        return true;
    }
    else return false;
}

bool storman_request(struct storman_item_t *smi) {
    if (xQueueSend(storman_queue, smi, 10) == pdTRUE) { // add request item to storagemanager queue, waiting 10 os ticks max
        return true;
    }
    else return false;
}

bool usb_data_get(uint8_t *usb_rx_data) {
    if (xQueueReceive(usb0_rx_queue, usb_rx_data, 0) == pdTRUE) { // try to get any data in queue immediately so as not to block
        return true;
    }
    else return false;
}

bool usb_data_put(uint8_t *usb_tx_data) {
    if (xQueueSend(usb0_tx_queue, usb_tx_data, 10) == pdTRUE) { // add data item to usb tx queue, waiting 10 os ticks max
        return true;
    }
    else return false;
}