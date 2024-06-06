/******************************************************************************
 * @file hw_spi.c
 *
 * @brief Functions for configuring and interacting with the SPI peripherals.
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
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "semphr.h"

// global SPI0 mutex
SemaphoreHandle_t spi0_mutex;


// chip select assertion routine to use in spi reads/writes
static inline void cs_assert(uint8_t cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop");
}

// chip select de-assertion routine to use in spi reads/writes
static inline void cs_deassert(uint8_t cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop");
}

void spi0_init(void) {
    // create SPI0 mutex
    spi0_mutex = xSemaphoreCreateMutex();

    // initialize SPI0 and set pins
    spi_init(SPI0_ID, SPI0_FREQ_KHZ * 1000);
    gpio_set_function(SPI0_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_CLK_PIN, GPIO_FUNC_SPI);

    // initialize chip select and drive high (it is active low)
    // for multiple devices on the bus, more chip selects would need to be added
    gpio_init(SPI0_CS_PIN_DEFAULT);
    gpio_set_dir(SPI0_CS_PIN_DEFAULT, GPIO_OUT);
    gpio_put(SPI0_CS_PIN_DEFAULT, 1);
}

int spi0_write_register(uint8_t cs_pin, uint8_t reg_addr, uint8_t data_byte) {
    int bytes_written;
    uint8_t write_buf[2];
    write_buf[0] = reg_addr & 0x7F; // write bit mask (MSB signifies read)
    write_buf[1] = data_byte;
    // obtain mutex on hw and then write to spi
    if(xSemaphoreTake(spi0_mutex, 10) == pdTRUE) {
        cs_assert(cs_pin);
        bytes_written = spi_write_blocking(SPI0_ID, write_buf, 2);
        cs_deassert(cs_pin);
        xSemaphoreGive(spi0_mutex);
    }
    if (bytes_written == 2) {
        return 1;
    }
    else return 0;
}

int spi0_read_registers(uint8_t cs_pin, uint8_t reg_addr, uint8_t *read_buf, uint16_t len) {
    int bytes_read;
    reg_addr |= 0x80; // make sure read bit is set (MSB)
    // obtain mutex on hw and then read from spi
    if(xSemaphoreTake(spi0_mutex, 10) == pdTRUE) {
        cs_assert(cs_pin);
        spi_write_blocking(SPI0_ID, &reg_addr, 1); // write the address to read
        wait_here_us(10 * 1000);                   // wait 10ms before read
        bytes_read = spi_read_blocking(SPI0_ID, 0, read_buf, len); // read the result
        cs_deassert(cs_pin);
        xSemaphoreGive(spi0_mutex);
    }
    return bytes_read;
}