/******************************************************************************
 * @file hw_uart.c
 *
 * @brief Functions for configuring and accessing the UART peripheral used for
 *        the Command Line Interface (CLI), or auxiliary UART for other uses.
 *        The implementation of these functions is MCU-specific and will need
 *        to be changed if ported to a new hardware family.
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
#include "shell.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "semphr.h"

#define NOCHAR 255 // return value of uart read if there is nothing to read


/************************
 * CLI UART functions
*************************/

static char cli_uart_rx_char = NOCHAR;

// global CLI UART mutex
SemaphoreHandle_t cli_uart_mutex;

// cli uart rx interrupt handler
static void on_cli_uart_rx() {
    if (uart_is_readable(UART_ID_CLI)) {
        cli_uart_rx_char = uart_getc(UART_ID_CLI);
    }
}

void cli_uart_init(void) {
    // The CLI UART is intended to be accessed character by character at low-ish
    // speed (human typing), with reads/writes regularly serviced by the CLI
    // task. As such it is configured without FIFO, and a simple rx interrupt
    // routine that puts the rx char into a global variable. It is assumed that
    // microshell will pull the byte before a new incoming byte overwrites it.

    // create CLI UART mutex
    cli_uart_mutex = xSemaphoreCreateMutex();
    
    // initialize uart at defined speed
    uart_init(UART_ID_CLI, UART_BAUD_RATE_CLI);

    // set tx and rx pins to the appropriate gpio function
    gpio_set_function(UART_TX_PIN_CLI, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_CLI, GPIO_FUNC_UART);

    // disable flow control
    uart_set_hw_flow(UART_ID_CLI, false, false);

    // set data format
    uart_set_format(UART_ID_CLI, UART_DATA_BITS_CLI, UART_STOP_BITS_CLI, UART_PARITY_CLI);

    // disable fifos - handle single characters only
    uart_set_fifo_enabled(UART_ID_CLI, false);

    // set up RX interrupt
    int UART_IRQ = UART_ID_CLI == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_cli_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID_CLI, true, false);

    cli_uart_rx_char = NOCHAR; // clear out RX buffer as a junk char appears upon enable

    // print out a string to indicate that uart was successfully initialized
    uart_puts(UART_ID_CLI, "\r\n\n");
    uart_puts(UART_ID_CLI, timestamp());
    uart_puts(UART_ID_CLI, "CLI UART initialized\r\n");
}

int cli_uart_putc(char tx_char) {
    int status = 0;
    if (uart_is_writable(UART_ID_CLI)) {
        if(xSemaphoreTake(cli_uart_mutex, 10) == pdTRUE) {
            // if uart is writeable and semaphore is obtained, write the char
            uart_putc_raw(UART_ID_CLI, tx_char);
            xSemaphoreGive(cli_uart_mutex);
            status = 1;
        }
    }
    return status;
}

char cli_uart_getc(void) {
    char ch;
    if (cli_uart_rx_char != NOCHAR) {
        // try to obtain the mutex to allow reading the buffered single char.
        // note that the direct UART read is inherently thread safe because it is
        // accessed thru a static ISR (on_cli_uart_rx).
        // of course this does not prevent another incoming UART char from overwriting
        // the buffered char if it is not pulled out in time.
        if(xSemaphoreTake(cli_uart_mutex, 10) == pdTRUE) {
            ch = cli_uart_rx_char;
            cli_uart_rx_char = NOCHAR;
            xSemaphoreGive(cli_uart_mutex);
        }
        return ch;
    }
}

void cli_uart_puts(const char *print_string) {
    if(xSemaphoreTake(cli_uart_mutex, 10) == pdTRUE) {
        uart_puts(UART_ID_CLI, print_string);
        xSemaphoreGive(cli_uart_mutex);
    }
}


/************************
 * AUX UART functions
*************************/

// global Aux UART mutex
SemaphoreHandle_t aux_uart_mutex;

void aux_uart_init(void) {
    // The auxilliary uart is a multi-purpose serial interface intended to send
    // and receive multiple bytes at a time, without any specific servicing
    // routine implemented. The FIFO is enabled but there is no RX ISR; on
    // the RP2040 platform this means we have 32 bytes of hw buffer before data
    // starts getting overwritten by any more incoming bytes.
    // Refer to cli_uart_init() if the need to implement an RX ISR arises.

    // create Aux UART mutex
    aux_uart_mutex = xSemaphoreCreateMutex();

    // initialize uart at defined speed
    uart_init(UART_ID_AUX, UART_BAUD_RATE_AUX);

    // set tx and rx pins to the appropriate gpio function
    gpio_set_function(UART_TX_PIN_AUX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_AUX, GPIO_FUNC_UART);

    // disable flow control
    uart_set_hw_flow(UART_ID_AUX, false, false);

    // set data format
    uart_set_format(UART_ID_AUX, UART_DATA_BITS_AUX, UART_STOP_BITS_AUX, UART_PARITY_AUX);

    // enable fifos
    uart_set_fifo_enabled(UART_ID_AUX, true);
}

int aux_uart_write(uint8_t *tx_data, size_t tx_len) {
    int status = 0;
    if (uart_is_writable(UART_ID_AUX)) {
        if(xSemaphoreTake(aux_uart_mutex, 10) == pdTRUE) {
            // if uart is writeable and semaphore is obtained, write the data
            uart_write_blocking(UART_ID_AUX, tx_data, tx_len);
            xSemaphoreGive(aux_uart_mutex);
            status = 1;
        }
    }
    return status;
}

int aux_uart_read(uint8_t *rx_data, size_t rx_len) {
    int rx_pos = 0;

    // read from uart if there is data in the fifo, up to rx_len times
    while(uart_is_readable(UART_ID_AUX) && rx_pos < rx_len) {
        // first try to obtain mutex
        if(xSemaphoreTake(aux_uart_mutex, 10) == pdTRUE) {
            rx_data[rx_pos] = (uint8_t) uart_getc(UART_ID_AUX);
            xSemaphoreGive(aux_uart_mutex);
            rx_pos++;
        }
    }

    return rx_pos + 1;
}