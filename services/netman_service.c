/******************************************************************************
 * @file netman_service.c
 *
 * @brief networkmanager service implementation and FreeRTOS task creation.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 01-24-2025
 *
 * @copyright Copyright (c) 2025 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <string.h>
#include "hardware_config.h"
#include "rtos_utils.h"
#include "services.h"
#include "service_queues.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hw_wifi.h"
#include "hw_net.h"


static void prvNetworkManagerTask(void *pvParameters); // network manager task
TaskHandle_t xNetManTask;

struct netman_info_t nmi_glob; // global network manager status info

extern void shell_net_mount(void); // declared in this file so /net can be mounted on-the-fly

// main service function, creates FreeRTOS task from prvNetworkManagerTask
BaseType_t netman_service(void)
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(
        prvNetworkManagerTask,
        xstr(SERVICE_NAME_NETMAN),
        STACK_NETMAN,
        NULL,
        PRIORITY_NETMAN,
        &xNetManTask
    );

    // print timestamp value
    cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        cli_uart_puts("Network manager service started\r\n");
    } else {
        cli_uart_puts("Error starting the network manager service\r\n");
    }

    return xReturn;
}

static void prvNetworkManagerTask(void *pvParameters) {
    // network manager action queue item
    netman_action_t nm_action;

    // initialize the wireless module. Note that this is done here in the task
    // because many network stacks/libraries are FreeRTOS-specific and so need
    // to be initialized when the OS is already running.
    if (hw_wifi_init()) { // todo: wifi country should be configurable
        hw_wifi_enable_sta_mode();
        cli_print_timestamped("WiFi hardware ready to connect");
        shell_net_mount(); // create '/net' node in the shell
        netman_request(NETJOIN); // auto-connect (will fail if '/net/wifi setauth' never used)
    } else {
        cli_print_timestamped("WiFi init failed");
    }

    while(true) {
        // Check the networkmanager action queue to see if an item is available
        if (xQueueReceive(netman_action_queue, (void *)&nm_action, 0) == pdTRUE)
        {
            // determine what action to perform
            switch(nm_action)
            {
                case NETJOIN: // join a network (connect)
                    struct storman_item_t smi;
                    char *wifi_ssid;
                    char *wifi_pass;
                    bool continue_connect = false;

                    // make sure we aren't already connected
                    if (nmi_glob.status == HW_WIFI_STATUS_UP) {
                        cli_print_raw("already connected to network");
                        break;
                    }

                    // get network credentials from the filesystem
                    smi.action = CHKFILE;
                    strcpy(smi.sm_item_name, "wifi_auth");
                    storman_request(&smi);
                    // wait for storagemanager to provide semaphore indicating file exists
                    if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                        // read credentials file
                        smi.action = DUMPFILE;
                        strcpy(smi.sm_item_name, "wifi_auth");
                        storman_request(&smi);
                        // wait for storagemanager to provide semaphore indicating data is ready
                        if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                            // Split the credentials into SSID and password
                            wifi_ssid = strtok(smi_glob.sm_item_data, ",");
                            wifi_pass = strtok(NULL, ",");
                            // check for valid credentials
                            if (wifi_ssid == NULL || wifi_pass == NULL) {
                                cli_print_raw("invalid wifi credentials format");
                                break;
                            } else {
                                continue_connect = true;
                            }
                        }
                    } else {
                        // this will print if the semaphore was never given by storagemanager
                        cli_print_raw("no wifi credentials found");
                        break;
                    }

                    if (continue_connect) { // attempt to connect to the network
                        char connect_msg[60];
                        uint64_t start_time = get_time_us(); // used to set a timeout
                        bool timed_out = false;

                        sprintf(connect_msg, "connecting to %s network...", wifi_ssid);
                        if (hw_wifi_connect_async(wifi_ssid, wifi_pass, HW_WIFI_AUTH_WPA2_AES_PSK)) {
                            cli_print_raw(connect_msg);
                            while(hw_wifi_get_addr()->addr == 0) {
                                if (get_time_us() - start_time > 20000000) { // 20 seconds
                                    timed_out = true;
                                    hw_wifi_reset_connection();
                                    break;
                                }
                                vTaskDelay(100); // spin every 100 OS ticks until connection is up
                            }
                            if (!timed_out) {
                                nmi_glob.status = HW_WIFI_STATUS_UP;
                                char ip_msg[32];
                                snprintf(ip_msg, 32, "wifi connected: %s", ip4addr_ntoa(hw_wifi_get_addr()));
                                cli_print_raw(ip_msg);
                                nmi_glob.ip = hw_wifi_get_addr()->addr;
/* enable optional networking features */
#ifdef ENABLE_HTTPD
                                net_mdns_init();
                                net_httpd_stack_init();
                                char httpd_msg[40+strlen(CYW43_HOST_NAME)];
                                sprintf(httpd_msg, "web console accessible at http://%s.local", CYW43_HOST_NAME);
                                cli_print_raw(httpd_msg);
#endif
/* end enable optional networking features */
                            } else {
                                cli_print_raw("wifi connection timed out");
                            }
                        } else {
                            cli_print_raw("could not start wifi connection");
                        }
                    }
                    break;

                case NETLEAVE: // leave a network (disconnect)
                    if (nmi_glob.status == HW_WIFI_STATUS_UP) {
                        hw_wifi_reset_connection();
                        nmi_glob.status = HW_WIFI_STATUS_LINK_DOWN;
                        nmi_glob.ip = 0;
                        cli_print_raw("wifi disconnected");
                    } else {
                        cli_print_raw("not connected to a network");
                    }
                    break;
                default:
                    break;
            }
        }
        
        // update this task's schedule
        task_sched_update(REPEAT_NETMAN, DELAY_NETMAN);
    }
}
