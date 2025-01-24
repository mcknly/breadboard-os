
#include "hardware_config.h"
#include "rtos_utils.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hw_wifi.h"


static void prvNetworkManagerTask(void *pvParameters); // network manager task
TaskHandle_t xNetManTask;

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
    //cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        cli_print_raw("Network manager service started");
    } else {
        cli_print_raw("Error starting the network manager service");
        // note these will only print if FreeRTOS/CLI is already running
        // (won't print if network manager is a startup service)
    }

    return xReturn;
}

static void prvNetworkManagerTask(void *pvParameters) {
    bool connecting = false;
    cli_print_raw("initializing wifi...");
    // todo: wifi country should be configurable
    if (hw_wifi_init()) {
        cli_print_raw("wifi initialized");
        hw_wifi_enable_sta_mode();
        cli_print_raw("station mode enabled, attempting to join network");
        if (hw_wifi_connect_async(WIFI_SSID, WIFI_PWD, HW_WIFI_AUTH_WPA2_AES_PSK)) {
            cli_print_raw("scanning...");
            while(hw_wifi_get_status() != HW_WIFI_STATUS_JOINED) {
                vTaskDelay(100); // spin every 100 OS ticks until connected to WiFi
            }
            while(hw_wifi_get_addr()->addr == 0) {
                vTaskDelay(100); // spin every 100 OS ticks until we have an IP address
            }
            cli_print_raw("wifi connected: ");
            cli_print_raw(ip4addr_ntoa(hw_wifi_get_addr()));
        } else {
            cli_print_raw("could not start wifi connection");
        }
    } else {
        cli_print_raw("could not initialize wifi");
    }

    while(true) {
        // update this task's schedule
        task_sched_update(REPEAT_NETMAN, DELAY_NETMAN);
    }
}
