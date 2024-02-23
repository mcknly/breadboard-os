/******************************************************************************
 * @file device_drivers.c
 *
 * @brief Monolithic function to initialize all peripheral devices connected to
 *        the system. Calls individual device initialization functions from the
 *        other source files.
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

#include <stdbool.h>
#include "hardware_config.h"
#include "device_drivers.h"


void driver_init(void) {

    // print initialization status text for externally connected devices.
    // This will only be visible if connected to the CLI UART; if CLI over USB
    // is used it will not show initial boot prints because all CLI output is
    // via FreeRTOS tasks/queues, and the OS kernel is not running yet.
    uart_puts(UART_ID_CLI, timestamp());
    uart_puts(UART_ID_CLI, "Initializing connected devices:\r\n\t  {");

    if (HW_USE_SPI0 && BME280_ATTACHED) {
        uart_puts(UART_ID_CLI, " bme280");
        if (bme280_init()) {
            uart_puts(UART_ID_CLI, "[PASS]");
        }
        else {
            uart_puts(UART_ID_CLI, "[FAIL]");
        }
    }

    if (HW_USE_I2C0 && MCP4725_ATTACHED) {
        uart_puts(UART_ID_CLI, " mcp4725");
        if (mcp4725_init()) {
            uart_puts(UART_ID_CLI, "[PASS]");
        }
        else {
            uart_puts(UART_ID_CLI, "[FAIL]");
        }
    }

    uart_puts(UART_ID_CLI, " }\r\n");
}