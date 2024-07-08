
#include "hardware_config.h"
#include "services.h"
#include "rtos_utils.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hw_wifi.h"

static void prvWifiTask(__unused void *params);

TaskHandle_t xWifiTask;

BaseType_t wifi_service() {
    uart_puts(UART_ID_CLI, timestamp());

    BaseType_t retval = xTaskCreate(
            prvWifiTask,
            "WifiTask",
            configMINIMAL_STACK_SIZE,
            NULL,
            configMAX_PRIORITIES - 1,
            &xWifiTask);

    if (retval == pdPASS) {
        uart_puts(UART_ID_CLI, "Wifi task started.\n");
    } else {
        uart_puts(UART_ID_CLI, "Error starting wifi task.\r\n");
    }

    return retval;
}

static void prvWifiTask(__unused void *params) {
    bool connecting = false;
    uart_puts(UART_ID_CLI, timestamp());
    uart_puts(UART_ID_CLI, "initializing wifi...\r\n");
    // todo: wifi country should be configurable
    if (hw_wifi_init_with_country(HW_WIFI_COUNTRY_USA)) {
        uart_puts(UART_ID_CLI, timestamp());
        uart_puts(UART_ID_CLI, "wifi initialized\r\n");
        hw_wifi_enable_sta_mode();
        uart_puts(UART_ID_CLI, timestamp());
        uart_puts(UART_ID_CLI, "station mode enabled, attempting to join.\r\n");
        if (hw_wifi_connect(WIFI_SSID, WIFI_PASSWORD, HW_WIFI_AUTH_MIXED)) {
            uart_puts(UART_ID_CLI,timestamp());
            uart_puts(UART_ID_CLI,"wifi connected: ");
            uart_puts(UART_ID_CLI,ip4addr_ntoa(hw_wifi_get_addr()));
            uart_puts(UART_ID_CLI,"\n");
//            connecting = true;
        } else {
            uart_puts(UART_ID_CLI,"could not start wifi connection\r\n");
        }
    } else {
        uart_puts(UART_ID_CLI,"could not initialize wifi\r\n");
    }

    uart_puts(UART_ID_CLI, timestamp());;
    uart_puts(UART_ID_CLI,"Starting task manager.\r\n");

    taskman_service();

    char msg[50];
    for (int i = 0; i < 50; i++) {
        msg[i] = 0;
    }
    while(connecting) {
        vTaskDelay(portTICK_PERIOD_MS * 5000);
        hw_wifi_status_t status = hw_wifi_get_status();
        connecting = false;
        if (status == HW_WIFI_STATUS_UP) {
            uart_puts(UART_ID_CLI,"wifi connected: ");
            uart_puts(UART_ID_CLI,ip4addr_ntoa(hw_wifi_get_addr()));
            uart_puts(UART_ID_CLI,"\n");
        } else if (status == HW_WIFI_STATUS_BADAUTH) {
            uart_puts(UART_ID_CLI,"bad credentials for SSID '");
            uart_puts(UART_ID_CLI, WIFI_SSID);
            uart_puts(UART_ID_CLI, "'\n");
        } else if (status == HW_WIFI_STATUS_FAIL) {
            uart_puts(UART_ID_CLI,"wifi connection failed -- no reason given");
        } else {
            sprintf(msg, "WiFi connecting [%d]...\r\n", status);
            uart_puts(UART_ID_CLI,msg);

            connecting = true;
        }
    }

    vTaskDelete(NULL);
}
