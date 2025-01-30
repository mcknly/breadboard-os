/******************************************************************************
 * @file service_queues.h
 *
 * @brief Initialization and helper functions for interaction with the queues
 *        used by services (FreeRTOS tasks) for inter-task communications and
 *        data handling.
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

#ifndef SERVICE_QUEUES_H
#define SERVICE_QUEUES_H

#include <stdbool.h>
#include "hardware_config.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "lfs.h"
#ifdef HW_USE_WIFI
#include "hw_wifi.h"
#endif


/**********************************************************
 * CLI printing queue -
 * allows tasks other than CLI to print to the CLI UART/USB
***********************************************************/
extern QueueHandle_t print_queue;
// note: if any characters are entered after the prompt, the CLI will not pull
// from the print queue... so the queue depth represents how many output strings
// can be buffered in the queue until current user input is completed
#define PRINT_QUEUE_DEPTH       32
#define PRINT_QUEUE_ITEM_SIZE   BUF_OUT_SIZE


/*************************************************
 * Task manager queue -
 * used to pass commands for managing task control
**************************************************/
extern QueueHandle_t taskman_queue;
typedef enum tm_action_t {DELETE, SUSPEND, RESUME} tm_action_t; // action to take on task
typedef struct taskman_item_t {TaskHandle_t task; tm_action_t action;} taskman_item_t; // queue item - struct for taskID & action
#define TASKMAN_QUEUE_DEPTH     1
#define TASKMAN_QUEUE_ITEM_SIZE sizeof(taskman_item_t)


/*****************************************************************
 * Storage manager queue -
 * pass commands & data for interacting with onboard flash storage
******************************************************************/
extern QueueHandle_t storman_queue;
// storagemanager actions for interacting with littlefs
typedef enum {LSDIR,      // list directory contents
              MKDIR,      // create a new directory
              RMDIR,      // delete a directory and its contents
              MKFILE,     // create a new empty file
              RMFILE,     // delete a file
              DUMPFILE,   // dump entire contents of file
              READFILE,   // read a portion of a file
              WRITEFILE,  // overwrite a file
              APPENDFILE, // append data to end of file
              FILESTAT,   // get file statistics
              CHKFILE,    // check if a file exists without error
              FSSTAT,     // get filesystem statistics
              FORMAT      // format the filesystem
             } storman_action_t;
// the storagemanager queue item structure
typedef struct storman_item_t {storman_action_t action;
                               char sm_item_name[PATHNAME_MAX_LEN];   // file or directory name
                               lfs_soff_t sm_item_offset;             // offset in file to read/write
                               long sm_item_size;                     // size of data to read/write
                               char sm_item_data[FILE_SIZE_MAX];      // file input/output data
                               struct lfs_info sm_item_info;          // littlefs info structure
                              } storman_item_t;

// global structure to hold storagemanager status data
extern struct storman_item_t smi_glob;
// global storagemanager status binary semaphore - for data ready synchronization
extern SemaphoreHandle_t smi_glob_sem;

#define STORMAN_QUEUE_DEPTH     1
#define STORMAN_QUEUE_ITEM_SIZE sizeof(smi_glob)


#ifdef HW_USE_WIFI
/************************************************************
 * Network manager queue -
 * pass commands & data for interacting with network hardware
*************************************************************/
extern QueueHandle_t netman_action_queue;
// networkmanager actions for interacting with network hardware
typedef enum {NETJOIN, // join a network
              NETLEAVE // leave a network
             } netman_action_t;
// the networkmanager instance info structure
typedef struct netman_info_t {hw_wifi_status_t status; // network connection status
                              hw_wifi_ip_addr_t ip;    // current IP address (IPv4)
                             } netman_info_t;

// global structure to hold networkmanager status info
extern struct netman_info_t nmi_glob;

#define NETMAN_ACTION_QUEUE_DEPTH      1
#define NETMAN_ACTION_QUEUE_ITEM_SIZE sizeof(netman_action_t)
#endif /* HW_USE_WIFI */


/**********************************************************
 * USB device queue -
 * holds buffered data bytes to or from the TinyUSB service
***********************************************************/
extern QueueHandle_t usb0_rx_queue;
extern QueueHandle_t usb0_tx_queue;
#define USB0_RX_QUEUE_DEPTH     8
#define USB0_TX_QUEUE_DEPTH     8
#define USB0_RX_QUEUE_ITEM_SIZE CFG_TUD_CDC_RX_BUFSIZE
#define USB0_TX_QUEUE_ITEM_SIZE CFG_TUD_CDC_TX_BUFSIZE


/************************
 * Queue helper functions
*************************/

/**
* @brief Initialize all queues.
*
* This routine uses the FreeRTOS xQueueCreate API to create each queue used
* in the system. All queues are created even if the tasks that use them have
* not yet been started.
*
* @param none
*
* @return 0 upon success, 1 upon failure
*/
bool init_queues(void);

// queue helper functions

/**
* @brief Print a raw string out to the CLI.
*
* This function puts a string pointer into the CLI printing queue exactly as
* passed.
*
* @param string pointer to the string to put in the queue for printing
*
* @return nothing
*/
bool cli_print_raw(char *string);

/**
* @brief Print a timestamped string out to the CLI.
*
* This function prepends a timestamp onto the given string and then drops the
* new timestamped string pointer into the CLI printing queue.
*
* @param string pointer to the string to put in the queue for printing
*
* @return nothing
*/
bool cli_print_timestamped(char *string);

/**
* @brief Send a request to taskmanager.
*
* This helper function puts a taskmanager request item structure into the
* taskmanager service's queue for performing control actions on available
* services.
*
* @param tmi pointer to the taskmanager request item to put into the queue for
*            controlling services
*
* @return true if item successfully queued, otherwise false (queue full)
*/
bool taskman_request(struct taskman_item_t *tmi);

/**
* @brief Send a request to storagemanager.
*
* This helper function puts a storagemanager request item structure into the
* storagemanager service's queue for interaction with a filesystem.
*
* @param smi pointer to the storagemanger request item to put into the queue for
*            filesystem access
*
* @return true if item successfully queued, otherwise false (queue full)
*/
bool storman_request(struct storman_item_t *smi);

#ifdef HW_USE_WIFI
/**
* @brief Send a request to networkmanager.
*
* This helper function puts a networkemanager action request item into the
* networkmanager service's queue for interaction with a the network hardware.
*
* @param nma networkmanger request item to put into the queue for network
             hardware interaction
*
* @return true if item successfully queued, otherwise false (queue full)
*/
bool netman_request(netman_action_t nma);
#endif /* HW_USE_WIFI */

/**
* @brief Get any data bytes in the USB RX buffer queue.
*
* This function will copy data (if available) out of the USB device RX queue.
* This data is in the form of a byte array up to the size of the USB RX buffer.
*
* @param usb_rx_data pointer to the buffer to copy the data from the rx queue into
*
* @return true if item retrieved, otherwise false (queue empty)
*/
bool usb_data_get(uint8_t *usb_rx_data);

/**
* @brief Put data bytes into the USB TX buffer queue.
*
* This function will put data to send into the USB TX queue. This data is a
* null-terminated byte array up to the size of the USB TX buffer.
*
* @param usb_rx_data pointer to the null-terminated data buffer to copy into the tx queue
*
* @return true if item successfully queued, otherwise false (queue full)
*/
bool usb_data_put(uint8_t *usb_tx_data);


#endif /* SERVICES_QUEUES_H */