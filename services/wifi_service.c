
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
             1,
            &xWifiTask);

    if (retval == pdPASS) {
        cli_uart_puts("Wifi task started.\n");
    } else {
        cli_uart_puts("Error starting wifi task.\r\n");
    }

    return retval;
}

static void prvWifiTask(__unused void *params) {
    bool connecting = false;
    cli_uart_puts(timestamp());
    cli_uart_puts("initializing wifi...\r\n");
    // todo: wifi country should be configurable
    if (hw_wifi_init_with_country(HW_WIFI_COUNTRY_USA)) {
        cli_uart_puts(timestamp());
        cli_uart_puts("wifi initialized\r\n");
        hw_wifi_enable_sta_mode();
        cli_uart_puts(timestamp());
        cli_uart_puts("station mode enabled, attempting to join.\r\n");
        if (hw_wifi_connect(WIFI_SSID, WIFI_PASSWORD, HW_WIFI_AUTH_MIXED)) {
            cli_uart_puts(timestamp());
            cli_uart_puts("wifi connected: ");
            cli_uart_puts(ip4addr_ntoa(hw_wifi_get_addr()));
            cli_uart_puts("\n");
        } else {
            cli_uart_puts("could not start wifi connection\r\n");
        }
    } else {
        cli_uart_puts("could not initialize wifi\r\n");
    }

    vTaskDelete(NULL);
}
