/******************************************************************************
 * @file hardware_config.c
 *
 * @brief Monolithic function to initialize all hardware on the device. Calls
 *        individual hardware initialization functions from the other source
 *        files.
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
#include "hardware_config.h"

#ifdef USING_CYW43
#include "pico/cyw43_arch.h"
#endif

void hardware_init(void) {
    // mutexes for accessing hardware peripherals (created within each hw init function)
    SemaphoreHandle_t gpio_mutex = NULL;
    SemaphoreHandle_t cli_uart_mutex = NULL;
    SemaphoreHandle_t aux_uart_mutex = NULL;
    SemaphoreHandle_t i2c0_mutex = NULL;
    SemaphoreHandle_t spi0_mutex = NULL;
    SemaphoreHandle_t onboard_flash_mutex = NULL;
    SemaphoreHandle_t adc_mutex = NULL;
    SemaphoreHandle_t usb_mutex = NULL;

    // initialize the uart for cli/microshell first for status prints
    cli_uart_init();

    // on pico w, we need to initialize the wifi chip
#ifdef USING_CYW43
    if(cyw43_arch_init()) {
        uart_puts(UART_ID_CLI, timestamp());
        uart_puts(UART_ID_CLI, "Failed to initialize CYW43 hardware.\r\n");
    } else {
        uart_puts(UART_ID_CLI, timestamp());
        uart_puts(UART_ID_CLI, "Initialized onboard wireless module\r\n");
    }
#endif

    // get the last reset reason string
    char *reset_reason_string = get_reset_reason_string();

    // print the last reset reason
    uart_puts(UART_ID_CLI, timestamp());
    uart_puts(UART_ID_CLI, reset_reason_string);

    // print hardware initialization status text. This will only be visible if
    // connected to the CLI UART; if CLI over USB is used it will not show initial
    // boot prints because all CLI output is via FreeRTOS tasks/queues, and the
    // OS kernel is not running yet.
    uart_puts(UART_ID_CLI, timestamp());
    uart_puts(UART_ID_CLI, "Configuring MCU peripherals:\r\n\t  { ");

    // initialize the auxilliary uart
    if (HW_USE_AUX_UART) {
        aux_uart_init();
        uart_puts(UART_ID_CLI, "aux_uart ");
    }

    // initialize i2c peripheral(s)
    if (HW_USE_I2C0) {
        i2c0_init();
        uart_puts(UART_ID_CLI, "i2c0 ");
    }

    // initialize spi peripheral(s)
    if (HW_USE_SPI0) {
        spi0_init();
        uart_puts(UART_ID_CLI, "spi0 ");
    }

    // initialize the onboard LED gpio (if not a Pico W board)
    if (HW_USE_ONBOARD_LED) {
        onboard_led_init();
        uart_puts(UART_ID_CLI, "led ");
    }

    // initialize the ADC(s)
    if (HW_USE_ADC) {
        adcs_init();
        uart_puts(UART_ID_CLI, "adc ");
    }

    // initialize USB hardware
    if (HW_USE_USB | CLI_USE_USB) {
        usb_device_init();
        uart_puts(UART_ID_CLI, "usb ");
    }

    // initialize GPIO
    if (HW_USE_GPIO) {
        gpio_init_all();
        uart_puts(UART_ID_CLI, "gpio ");
    }

    // initialize onboard flash
    if (HW_USE_ONBOARD_FLASH) {
        onboard_flash_init();
        uart_puts(UART_ID_CLI, "flash ");
    }

    uart_puts(UART_ID_CLI, "}\r\n");
}
