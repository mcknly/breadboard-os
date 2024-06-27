/******************************************************************************
 * @file node_dev.c
 *
 * @brief /dev folder for the CLI, contains hardware peripheral access functions
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <microshell.h>
#include "hardware_config.h"
#include "shell.h"
#include "cli_utils.h"
#include "services.h"
#include "service_queues.h"
#include "lfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/**
* @brief '/dev/led' get data callback function.
*
* Returns the current state of the onboard LED using the built-in 'cat' CLI command.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the LED state data (0=OFF or 1=ON)
*/
size_t led_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    bool led_state;
    static char led_state_msg[16];

    if (onboard_led_get() == true) {
        strcpy(led_state_msg, "LED STATE ON\r\n");
    } else {
        strcpy(led_state_msg, "LED STATE OFF\r\n");
    }

    // set pointer to data
    *data = (uint8_t*)led_state_msg;
    // return data size
    return strlen(led_state_msg);
}

/**
* @brief '/dev/led' set data callback function.
*
* Sets the state of the onboard LED using the built-in 'echo' CLI command.
*
* @param ush_file_data_setter Params given by typedef ush_file_data_setter. see ush_types.h
*
* @return nothing
*/
void led_set_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t *data, size_t size)
{
    if (strcmp(data, "0") == 0) {
        onboard_led_set(0);
    } else if (strcmp(data, "1") == 0) {
        onboard_led_set(1);
    } else {
        shell_print("set value should be <0> or <1>");
    }
}

/**
* @brief '/dev/time' get data callback function.
*
* Returns the microseconds value of the system timer using the built-in 'cat' CLI command.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the timer value data
*/
size_t time_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    static char time_msg[57]; // text string plus 14 digits for timer value, ~3 years before rollover
    // of course... the 64-bit timer value can count to 585 thousand years,
    // in case we want to ever throw a couple more bytes at it for longetivity
    snprintf(time_msg, 57, "current system timer value: %llu microseconds\r\n", get_time_us());

    // set pointer to data
    *data = (uint8_t*)time_msg;
    // return data size
    return strlen(time_msg);
}

/**
* @brief '/dev/gpio' executable callback function.
*
* Read/write value (0, 1) to individual GPIO pins. Note that the 'GPIO num'
* supplied to the command should match the GPIO_x_MCU_ID index number configured
* in hardware_config.h.
*
* @param ush_file_execute_callback Params given by typedef ush_file_execute_callback. see ush_types.h
*
* @return nothing
*/
static void gpio_exec_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    bool syntax_err = false;
    char *gpio_msg = pvPortMalloc(20);

    if (strcmp(argv[1], "read") == 0 && argc == 3) {
        int gpio_index = atoi(argv[2]);
        // make sure the GPIO number provided is in range
        if (gpio_index < GPIO_COUNT) {
            sprintf(gpio_msg, "GPIO_%d value: %d", gpio_index, gpio_read_single(gpio_index));
        }
        else {
            syntax_err = true;
            shell_print("pin is not a configured GPIO");
        }
    }
    else if (strcmp(argv[1], "write") == 0 && argc == 4) {
        int gpio_index = atoi(argv[2]);
        int gpio_val = atoi(argv[3]);

        // make sure the value is 0 or 1
        if (gpio_val == 0 || gpio_val == 1) {
            // make sure the GPIO number provided is in range
            if (gpio_index < GPIO_COUNT && gpio_settings.gpio_direction[gpio_index] == GPIO_OUT) {
                gpio_write_single(gpio_index, gpio_val);
                sprintf(gpio_msg, "GPIO_%d set to %d", gpio_index, gpio_read_single(gpio_index));
            }
            else {
                syntax_err = true;
                shell_print("pin is not a configured GPIO output");
            }
        }
        else {
            syntax_err = true;
            shell_print("value must be 0 or 1");
        }
    }
    else {
        syntax_err = true;
        shell_print("command syntax error, see 'help <gpio>'");
    }

    if (!syntax_err) {
        shell_print(gpio_msg);
    }
    vPortFree(gpio_msg);
}

/**
* @brief '/dev/gpio' get data callback function.
*
* Prints the value of all configured GPIOs and their directions.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t gpio_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    uint32_t gpio_values;
    char *gpio_states_msg = pvPortMalloc(80 + 20 * GPIO_COUNT);
    char direction[4];
    
    strcpy(gpio_states_msg,
            USH_SHELL_FONT_STYLE_BOLD
            USH_SHELL_FONT_COLOR_BLUE
            "GPIO_ID\t\tDirection\tValue\r\n"
            "-------------------------------------\r\n"
            USH_SHELL_FONT_STYLE_RESET);
    
    for (int gpio_num = 0; gpio_num < GPIO_COUNT; gpio_num++) {
        if (gpio_settings.gpio_direction[gpio_num] == GPIO_IN) {
            strcpy(direction, "IN");
        }
        else {
            strcpy(direction, "OUT");
        }
        sprintf(gpio_states_msg + strlen(gpio_states_msg),
                    "GPIO_%d\t\t%s\t\t%d\r\n",
                    gpio_num,
                    direction,
                    gpio_read_single(gpio_num)
                );
    }
    
    // print directly from this function rather than returning pointer to uShell.
    // this allows us to malloc/free rather than using static memory
    shell_print(gpio_states_msg);
    vPortFree(gpio_states_msg);
    // return null since we already printed output
    return 0;
}

/**
* @brief '/dev/i2c0' executable callback function.
*
* Interact with I2C bus 0 by reading/writing raw bytes targetted to a specific
* client device address.
*
* @param ush_file_execute_callback Params given by typedef ush_file_execute_callback. see ush_types.h
*
* @return nothing
*/
static void i2c0_exec_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    bool syntax_err = false;

    if (argc == 4 &&
        (strcmp(argv[1], "read") == 0 || strcmp(argv[1], "write") == 0) &&
        (argv[2][0] == '0' && argv[2][1] == 'x')) {

        // get i2c target address
        uint8_t addr = strtol(&argv[2][2], NULL, 16);

        // i2c read
        if (strcmp(argv[1], "read") == 0) {
            size_t nbytes = strtol(argv[3], NULL, 10);
            uint8_t rxdata[nbytes];

            // read data from i2c bus
            if (i2c0_read(addr, rxdata, nbytes) > 0) {
                char *rx_msg_prefix = "Received: 0x";
                // allocate heap memory for printable rx data
                char *rx_msg = pvPortMalloc(nbytes * 3 + strlen(rx_msg_prefix));
                sprintf(rx_msg, rx_msg_prefix);

                for (int rx_byte = 0; rx_byte < nbytes; rx_byte++) {
                    sprintf(rx_msg + strlen(rx_msg_prefix) + (rx_byte * 3), "%02x ", rxdata[rx_byte]);
                }

                shell_print(rx_msg);
                vPortFree(rx_msg);
            }
            else {
                shell_print("No response");
            }
        }

        // i2c write
        if (strcmp(argv[1], "write") == 0) {
            // guess the length of the data to write before actually checking
            size_t nbytes = (strlen(argv[3]) - 2) / 2;
            uint8_t txdata[nbytes];
            // convert string to data and get real number of bytes
            nbytes = hex_string_to_byte_array(argv[3], txdata);
            // write data on i2c bus if formatted correctly
            if (nbytes > 0) {
                int bytes_written = i2c0_write(addr, txdata, nbytes);
                if(bytes_written > 0) {
                    char *tx_msg = pvPortMalloc(16);
                    sprintf(tx_msg, "Wrote %d bytes", bytes_written);
                    shell_print(tx_msg);
                    vPortFree(tx_msg);
                }
                else {
                    shell_print("Error writing to bus");
                }
            }
            else {syntax_err = true;}
        }
    }
    else {syntax_err = true;}

    if (syntax_err) {
        shell_print("command syntax error, see 'help <i2c0>'");
    }
}

/**
* @brief '/dev/i2c0' get data callback function.
*
* Sweep through all 7-bit I2C addresses, to see if any client devices are present
* on the I2C bus. Print out a table that looks like this:
*
* I2C Bus Scan
*   0 1 2 3 4 5 6 7 8 9 A B C D E F
* 0
* 1     @
* 2
* 3         @
* 4
* 5
* 6
* 7
*
* E.g. if peripheral device addresses 0x12 and 0x34 were acknowledged.
* Functional idea borrowed from:
* https://github.com/raspberrypi/pico-examples/blob/master/i2c/bus_scan/bus_scan.c
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t i2c0_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // use malloc rather than passing a pointer to a static char back to uShell,
    // since it is a rather large array
    char *i2c0_scan_msg = pvPortMalloc(400);

    sprintf(i2c0_scan_msg,  USH_SHELL_FONT_STYLE_BOLD
                            USH_SHELL_FONT_COLOR_BLUE
                            "I2C0 Bus Scan\r\n"
                            USH_SHELL_FONT_STYLE_RESET
                            "   0 1 2 3 4 5 6 7 8 9 A B C D E F\r\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            sprintf(i2c0_scan_msg + strlen(i2c0_scan_msg), "%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a peripheral device
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.
        int ret;
        uint8_t rxdata;
        if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78) // skip over i2c "reserved" 0000xxx or 1111xxx addresses
            ret = -1;
        else
            ret = i2c0_read(addr, &rxdata, 1);

        sprintf(i2c0_scan_msg + strlen(i2c0_scan_msg), ret < 0 ? "." : "@");
        sprintf(i2c0_scan_msg + strlen(i2c0_scan_msg), addr % 16 == 15 ? "\r\n" : " ");
    }

    // print directly from this function rather than returning pointer to uShell.
    // this allows us to malloc/free rather than using static memory
    shell_print(i2c0_scan_msg);
    vPortFree(i2c0_scan_msg);
    // return null since we already printed output
    return 0;
}

/**
* @brief '/dev/spi0' executable callback function.
*
* Interact with SPI bus 0 by reading/writing to a specific register address.
*
* @param ush_file_execute_callback Params given by typedef ush_file_execute_callback. see ush_types.h
*
* @return nothing
*/
static void spi0_exec_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    bool syntax_err = false;

    if (argc == 4 &&
        (strcmp(argv[1], "read") == 0 || strcmp(argv[1], "write") == 0) &&
        (argv[2][0] == '0' && argv[2][1] == 'x')) {

        // get spi target register address
        uint8_t addr = strtol(&argv[2][2], NULL, 16);

        // spi read
        if (strcmp(argv[1], "read") == 0) {
            size_t nbytes = strtol(argv[3], NULL, 10);
            uint8_t rxdata[nbytes];

            // read data from spi bus
            if (spi0_read_registers(SPI0_TARGET_DEV_0_CS, addr, rxdata, nbytes) > 0) {
                char *rx_msg_prefix = "Received: 0x";
                // allocate heap memory for printable rx data
                char *rx_msg = pvPortMalloc(nbytes * 3 + strlen(rx_msg_prefix));
                sprintf(rx_msg, rx_msg_prefix);

                for (int rx_byte = 0; rx_byte < nbytes; rx_byte++) {
                    sprintf(rx_msg + strlen(rx_msg_prefix) + (rx_byte * 3), "%02x ", rxdata[rx_byte]);
                }

                shell_print(rx_msg);
                vPortFree(rx_msg);
            }
            else {
                shell_print("No response");
            }
        }

        // spi write
        if (strcmp(argv[1], "write") == 0) {
            // guess the length of the data to write before actually checking
            size_t nbytes = (strlen(argv[3]) - 2) / 2;
            uint8_t txdata[nbytes];
            // convert string to data and get real number of bytes
            nbytes = hex_string_to_byte_array(argv[3], txdata);
            // write data on i2c bus if formatted correctly
            if (nbytes == 1) { // for now we are only expecting to write a single byte to a single register addr
                int bytes_written = spi0_write_register(SPI0_TARGET_DEV_0_CS, addr, txdata[0]);
                if(bytes_written > 0) {
                    char *tx_msg = pvPortMalloc(16);
                    sprintf(tx_msg, "Wrote %d bytes", bytes_written);
                    shell_print(tx_msg);
                    vPortFree(tx_msg);
                }
                else {
                    shell_print("Error writing to bus");
                }
            }
            else {syntax_err = true;}
        }
    }
    else {syntax_err = true;}

    if (syntax_err) {
        shell_print("command syntax error, see 'help <i2c0>'");
    }
}

/**
* @brief '/dev/spi0' get data callback function.
*
* Interrogates any devices on SPI bus 0 for its ID number.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t spi0_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // read out the device ID of expected target on the bus
    // device ID specified in hardware_config.h
    // if more than one device is attached to the bus, this can be made into a loop through chip selects
    uint8_t device_id;
    int bytes_read = spi0_read_registers(SPI0_TARGET_DEV_0_CS, SPI0_TARGET_DEV_0_ID_REG, &device_id, 1);
    char *device_id_msg = pvPortMalloc(30);
    if (bytes_read == 1 && device_id != 0) { // assuming device ID is never 0, that would be weird
        sprintf(device_id_msg, "found device id: 0x%x\r\n", device_id);
    }
    else {
        sprintf(device_id_msg, "no response on SPI\r\n");
    }
    
    // print directly from this function rather than returning pointer to uShell.
    // this allows us to malloc/free rather than using static memory
    shell_print(device_id_msg);
    vPortFree(device_id_msg);

    // return null since we already printed output
    return 0;
}

/**
* @brief '/dev/adc' get data callback function.
*
* Reads the value of the ADC peripheral and prints the value converted to a
* voltage. See the settings in hardware_config.h for which channel/pin this
* corresponds to.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the ADC value
*/
size_t adc0_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    static char adc_val[9] = {'\0'};

    sprintf(adc_val, "%.3fV\r\n", read_adc(0));

    // set pointer to data
    *data = (uint8_t*)adc_val;
    // return data size
    return strlen(adc_val);
}

/**
* @brief '/dev/usb0' get data callback function.
*
* Reads any bytes that are available in the USB device RX buffer, and attempts
* to print them as a string. This of course assumes the other end is sending a
* proper string.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t usb0_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    uint8_t *usb_rx_data = pvPortMalloc(CFG_TUD_CDC_RX_BUFSIZE);
    usb_rx_data[0] = '\0'; // make sure initial strlen is zero
    
    while (usb_data_get(usb_rx_data + strlen(usb_rx_data))) {
        // try to get all data from usb rx queue
    }
    
    // print directly from this function rather than returning pointer to uShell.
    // this allows us to malloc/free rather than using static memory
    shell_print(usb_rx_data);
    vPortFree(usb_rx_data);
    // return null since we already printed output
    return 0;
}

/**
* @brief '/dev/usb0' set data callback function.
*
* Puts bytes (string) into the USB device TX buffer to send to the host endpoint.
*
* @param ush_file_data_setter Params given by typedef ush_file_data_setter. see ush_types.h
*
* @return nothing
*/
void usb0_set_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t *data, size_t size)
{
    usb_data_put(data);
}

/**
* @brief '/dev/uart1' get data callback function.
*
* Reads any bytes that are available in the auxilliary UART RX FIFO, and attempts
* to print them as a string.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the UART data byte array
*/
size_t uart1_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    static uint8_t uart_rx_data[UART_RX_FIFO_SIZE_AUX];
    uart_rx_data[0] = '\0'; // make sure initial strlen is zero

    // set pointer to data
    *data = (uint8_t*)uart_rx_data;

    if (aux_uart_read(uart_rx_data, UART_RX_FIFO_SIZE_AUX) > 0) {
        // return data size
        return strlen(uart_rx_data);
    }
    else {
        return 0;
    }
}

/**
* @brief '/dev/uart1' set data callback function.
*
* Puts bytes (string) into the UART TX FIFO.
*
* @param ush_file_data_setter Params given by typedef ush_file_data_setter. see ush_types.h
*
* @return nothing
*/
void uart1_set_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t *data, size_t size)
{
    aux_uart_write(data, size);
}

// dev directory files descriptor
static const struct ush_file_descriptor dev_files[] = {
#if HW_USE_ONBOARD_LED
    {
        .name = "led",                          // file name (required)
        .description = "onboard LED",           // optional file description
        .help = NULL,                           // optional help manual
        .exec = NULL,                           // optional execute callback
        .get_data = led_get_data_callback,      // optional get data (cat) callback
        .set_data = led_set_data_callback       // optional set data (echo) callback
    },
#endif /* HW_USE_ONBOARD_LED */
    {
        .name = "time",
        .description = "system timer",
        .help = NULL,
        .exec = NULL,
        .get_data = time_get_data_callback,
        .set_data = NULL
    },
#if HW_USE_GPIO
    {
        .name = "gpio",
        .description = "GPIO pins",
        .help = "usage: gpio <read>  <\e[3mGPIO num\e[0m>\r\n"
                "            <write> <\e[3mGPIO num\e[0m> <\e[3mvalue\e[0m>\r\n"
                "\r\n"
                "       cat gpio - print all GPIO states\r\n",
        .exec = gpio_exec_callback,
        .get_data = gpio_get_data_callback,
        .set_data = NULL
    },
#endif /* HW_USE_GPIO */
#if HW_USE_I2C0
    {
        .name = "i2c0",
        .description = "I2C bus 0",
        .help = "usage: i2c0 <read>  <\e[3maddress(0x...)\e[0m> <\e[3mnbytes\e[0m>\r\n"
                "            <write> <\e[3maddress(0x...)\e[0m> <\e[3mdata(0x...)\e[0m>\r\n"
                "\r\n"
                "       cat i2c0 - scan i2c0 bus and print a table of responding addresses\r\n",
        .exec = i2c0_exec_callback,
        .get_data = i2c0_get_data_callback,
        .set_data = NULL
    },
#endif /* HW_USE_I2C0 */
#if HW_USE_SPI0
    {
        .name = "spi0",
        .description = "SPI bus 0",
        .help = "usage: spi0 <read>  <\e[3mreg addr(0x...)\e[0m> <\e[3mnbytes\e[0m>\r\n"
                "            <write> <\e[3mreg addr(0x...)\e[0m> <\e[3mdata byte(0x...)\e[0m>\r\n"
                "\r\n"
                "       cat spi0 - read device IDs of all devices in chip select table\r\n",
        .exec = spi0_exec_callback,
        .get_data = spi0_get_data_callback,
        .set_data = NULL
    },
#endif /* HW_USE_SPI0 */
#if HW_USE_ADC && ADC0_INIT
    {
        .name = "adc0",
        .description = "Analog-to-Digital Converter",
        .help = NULL,
        .exec = NULL,
        .get_data = adc0_get_data_callback,
        .set_data = NULL
    },
#endif /* HW_USE_ADC && ADC0_INIT */
#if HW_USE_USB && !CLI_USE_USB
    {
        .name = "usb0",
        .description = "USB data interface",
        .help = NULL,
        .exec = NULL,
        .get_data = usb0_get_data_callback,
        .set_data = usb0_set_data_callback
    },
#endif /* HW_USE_USB && !CLI_USE_USB */
#if HW_USE_AUX_UART
    {
        .name = "uart1",
        .description = "auxilliary UART",
        .help = NULL,
        .exec = NULL,
        .get_data = uart1_get_data_callback,
        .set_data = uart1_set_data_callback
    }
#endif /* HW_USE_AUX_UART */
};

// dev directory handler
static struct ush_node_object dev;

void shell_dev_mount(void)
{
    // mount dev directory
    ush_node_mount(&ush, "/dev", &dev, dev_files, sizeof(dev_files) / sizeof(dev_files[0]));
}