/******************************************************************************
 * @file mcp4725.c
 *
 * @brief Hardware interface driver functions for the Microchip MCP4725 12-bit
 *        Digital-to-Analog Converter (DAC) with built-in EEPROM.
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

#include <stdint.h>
#include "device_drivers.h"
#include "hardware_config.h"


int mcp4725_init(void) {
    uint8_t rx_byte;

    // read a single byte to check for MCP4725 device code
    if (i2c0_read(MCP4725_I2C_ADDR, &rx_byte, 1) == 1) {
        rx_byte = rx_byte >> 4;
        if (rx_byte == 0xC) { // check for 4-bit device code 1100
            return 1;
        }
        else return 0;
    }
}

int mcp4725_set_voltage(float voltage, bool save_in_eeprom) {
    uint16_t dac_setting = (uint16_t)(4095 * voltage / MCP4725_VDD);
    uint8_t dac_write_data[3];

    if (save_in_eeprom) {
        dac_write_data[0] = 0x60; // specify write to DAC register and EEPROM in 1st byte
        dac_write_data[1] = (uint8_t) (dac_setting >> 4); // upper 8 bits of DAC value goes in 2nd byte
        dac_write_data[2] = (uint8_t) (dac_setting << 4); // lower 4 bits goes in top nibble of 3rd byte

        if (i2c0_write(MCP4725_I2C_ADDR, dac_write_data, 3) == 3) {
            return 1;
        }
        else return 0;
    }
    else {
        dac_write_data[0] = (uint8_t) (dac_setting >> 8); // upper 4 bits of DAC value goes in 1st byte
        dac_write_data[1] = (uint8_t) dac_setting;        // lower 8 bits of DAC value goes into 2nd byte

        if (i2c0_write(MCP4725_I2C_ADDR, dac_write_data, 2) == 2) {
            return 1;
        }
        else return 0;
    }
}

float mcp4725_get_voltage(void) {
    uint16_t dac_setting;
    uint8_t dac_read_data[3];

    if (i2c0_read(MCP4725_I2C_ADDR, dac_read_data, 3) == 3) {
        dac_setting = (uint16_t) (dac_read_data[1] << 4);               // 2nd byte read is 8 MSBs of the current DAC setting
        dac_setting = dac_setting | (uint16_t) (dac_read_data[2] >> 4); // top nibble of 3rd byte read has 4 LSBs of DAC setting

        float voltage = (float) MCP4725_VDD * (float) dac_setting / 4095;
        return voltage;
    }
    else return -1;
}