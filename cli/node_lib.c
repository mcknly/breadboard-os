/******************************************************************************
 * @file node_lib.c
 *
 * @brief /lib folder for the CLI, contains hardware peripheral driver interfaces
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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <microshell.h>
#include "hardware_config.h"
#include "device_drivers.h"
#include "shell.h"


/**
* @brief '/lib/bme280' get data callback function.
*
* Print readings from Bosch BME280 environmental sensor peripheral.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the sensor readings string
*/
size_t bme280_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    bme280_sensor_data_t sensor_data;
    static char bme280_data_msg[60];

    // read compensation data from device and then get sensor readings
    if(bme280_read_sensors(&bme280_compensation_params_glob, &sensor_data)) {
        sprintf(bme280_data_msg,
                "Temp:\t %.1f degC\r\n"
                "Hum:\t %.1f%%\r\n"
                "Pres:\t %.1f hPa\r\n",
                sensor_data.temperature,
                sensor_data.humidity,
                sensor_data.pressure);
    }
    else {
        sprintf(bme280_data_msg, "error reading sensor\r\n");
    }

    // set pointer to data
    *data = (uint8_t*)bme280_data_msg;
    // return data size
    return strlen(bme280_data_msg);
}

/**
* @brief '/lib/mcp4725' get data callback function.
*
* Prints the current MCP4725 DAC setting converted to a voltage.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the voltage setting string
*/
size_t mcp4725_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    static char mcp4725_data_msg[30];
    float voltage;

    voltage = mcp4725_get_voltage();
    if (voltage >= 0) {
        sprintf(mcp4725_data_msg, "%.2fV\r\n", voltage);
    }
    else {
        strcpy(mcp4725_data_msg, "error reading DAC value\r\n");
    }

    // set pointer to data
    *data = (uint8_t*)mcp4725_data_msg;
    // return data size
    return strlen(mcp4725_data_msg);
}

/**
* @brief '/lib/mcp4725' set data callback function.
*
* Sets the DAC output register given a user-defined voltage.
*
* @param ush_file_data_setter Params given by typedef ush_file_data_setter. see ush_types.h
*
* @return nothing
*/
void mcp4725_set_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t *data, size_t size)
{
    float voltage_setting = strtof(data, NULL);
    if (voltage_setting >= 0 && voltage_setting <= MCP4725_VDD) {
        mcp4725_set_voltage(voltage_setting, false);
    }
}

// lib directory files descriptor
static const struct ush_file_descriptor lib_files[] = {
    #if HW_USE_SPI0 && BME280_ATTACHED
    {
        .name = "bme280",                                       // file name (required)
        .description = "Bosch BME280 environmental sensor",     // optional file description
        .help = NULL,                                           // optional help manual
        .exec = NULL,                                           // optional execute callback
        .get_data = bme280_get_data_callback,                   // optional get data (cat) callback
        .set_data = NULL                                        // optional set data (echo) callback
    },
    #endif
    #if HW_USE_I2C0 && MCP4725_ATTACHED
    {
        .name = "mcp4725",
        .description = "Microchip MCP4725 digital-to-analog converter",
        .help = "usage: cat mcp4725 - get current voltage setting of DAC\r\n"
                "       echo \e[3mx.xxx\e[0m > mcp4725 - set DAC voltage\r\n",
        .exec = NULL,
        .get_data = mcp4725_get_data_callback,
        .set_data = mcp4725_set_data_callback
    }
    #endif
};

// lib directory handler
static struct ush_node_object lib;

void shell_lib_mount(void)
{
    // mount dev directory
    ush_node_mount(&ush, "/lib", &lib, lib_files, sizeof(lib_files) / sizeof(lib_files[0]));
}