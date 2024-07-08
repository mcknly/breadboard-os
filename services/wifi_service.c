
#include "hardware_config.h"
#include "services.h"
#include "rtos_utils.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hw_wifi.h"

static void prvWifiTask(__unused void *params);

TaskHandle_t xWifiTask;

BaseType_t wifi_service() {
    cli_uart_puts(timestamp());

    BaseType_t retval = xTaskCreate(
            prvWifiTask,
            "WifiTask",
            configMINIMAL_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY + 1,
            NULL);

    if (retval == pdPASS) {
        cli_uart_puts("Wifi task started.\r\n");
    } else {
        cli_uart_puts("Error starting wifi task.\r\n");
    }

    return retval;
}

static void prvWifiTask(__unused void *params) {
    bool connecting = false;
    puts("initializing wifi");
    if (hw_wifi_init_with_country(HW_WIFI_COUNTRY_USA)) {
        puts("wifi initialized");
        hw_wifi_enable_sta_mode();
        puts("station mode enabled");
        if (hw_wifi_connect(WIFI_SSID, WIFI_PASSWORD, HW_WIFI_AUTH_MIXED)) {
            puts("wifi connection initiated");
            connecting = true;
        } else {
            puts("could not start wifi connection");
        }
    } else {
        puts("could not initialize wifi");
    }

    while(connecting) {
        vTaskDelay(portTICK_PERIOD_MS * 5);
        hw_wifi_status_t status = hw_wifi_get_status();
        connecting = false;
        if (status == HW_WIFI_STATUS_UP) {
            puts("wifi connected");
            puts(ip4addr_ntoa(hw_wifi_get_addr()));
        } else if (status == HW_WIFI_STATUS_BADAUTH) {
            printf("bad credentials for SSID '%s'\n", WIFI_SSID);
        } else if (status == HW_WIFI_STATUS_FAIL) {
            puts("wifi connection failed -- no reason given");
        } else {
            puts("WiFi connecting...");
            connecting = true;
        }
    }

    vTaskDelete(NULL);
}
