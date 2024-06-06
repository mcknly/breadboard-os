/******************************************************************************
 * @file hw_i2c.c
 *
 * @brief Functions for configuring and interacting with the I2C peripherals.
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
#include "hardware/i2c.h"
#include "semphr.h"

// global I2C0 mutex
SemaphoreHandle_t i2c0_mutex;


void i2c0_init(void) {
    // create I2C0 mutex
    i2c0_mutex = xSemaphoreCreateMutex();

    i2c_init(I2C0_ID, I2C0_FREQ_KHZ * 1000);
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA_PIN);
    gpio_pull_up(I2C0_SCL_PIN);
}

int i2c0_write(uint8_t addr, const uint8_t *write_data, size_t length) {
    i2c_inst_t *i2c = I2C0_ID;
    int bytes_written = 0;

    // write bytes if mutex is obtained
    if(xSemaphoreTake(i2c0_mutex, 10) == pdTRUE) {
        bytes_written = i2c_write_blocking(i2c, addr, write_data, length, false);
        xSemaphoreGive(i2c0_mutex);
    }

    // check for Pico SDK-specific error and replace with -1
    if(bytes_written == PICO_ERROR_GENERIC) {
        bytes_written = -1;
    }

    return bytes_written;
}

int i2c0_read(uint8_t addr, uint8_t *read_data, size_t length) {
    i2c_inst_t *i2c = I2C0_ID;
    int bytes_read = 0;

    // read bytes if mutex is obtained
    if(xSemaphoreTake(i2c0_mutex, 10) == pdTRUE) {
        bytes_read = i2c_read_blocking(i2c, addr, read_data, length, false);
        xSemaphoreGive(i2c0_mutex);
    }

    // check for Pico SDK-specific error and replace with -1
    if(bytes_read == PICO_ERROR_GENERIC) {
        bytes_read = -1;
    }

    return bytes_read;
}