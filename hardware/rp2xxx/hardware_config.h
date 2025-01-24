/******************************************************************************
 * @file hardware_config.h
 *
 * @brief Settings and function prototypes for all interaction with MCU
 *        hardware. Hardware-specific implementation resides within these
 *        functions - it is intended that porting to a new MCU will require
 *        changes only to the function implementations that reside in the
 *        /hardware_<mcu> folder.
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

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include "shell.h"
#include "lfs.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"


// global MCU name - used throughout the codebase to differentiate architectures
#ifdef USING_RP2350
#define MCU_NAME RP2350_Cortex_M33
#else
#define MCU_NAME RP2040_Cortex_M0
#endif

// stringify helper - use xstr() to convert #define to a usable string
#define str(s) # s
#define xstr(s) str(s)


/************************
 * TABLE OF CONTENTS
*************************
search file for these keywords including the leading '*' to find section
 * GPIO
 * CLI UART
 * Auxilliary UART
 * I2C0 Master
 * SPI0 Master
 * On-board LED
 * Watchdog Timer
 * Chip Reset
 * System Clocks/Timers
 * System MCU Cores
 * Chip Versions
 * Chip Registers
 * Onboard Flash
 * ADC - Analog-to-Digital Coverters
 * USB (TinyUSB) CDC
 * Wireless (CYW43)


/**
* @brief Initialize all used hardware peripherals.
*
* Single function to initialize & configure hardware & peripherals on the device,
* to be run once at boot.
*
* @param none
*
* @return nothing
*/
void hardware_init(void);


/************************
 * GPIO
************************/

/*GPIO Settings
*
* Each GPIO pin should have its own entry as `GPIO_x_` in
* the following parameter lists:
*     MCU_ID
*     DIRECTION
*     PULL
*     IRQ_EN
* After each set of defines, an ordered list is created with each of the GPIO
* indexes, using the following example:
*     #define GPIO_MCU_IDS GPIO_0_MCU_ID, \   <-- note the backslash for preprocessor line break
*                          GPIO_1_MCU_ID, \
*                          GPIO_2_MCU_ID, \
*                          GPIO_3_MCU_ID
* This allows population in the array struct definition in hw_gpio.c without
* needing to update that source file. Changes to add remove GPIO only need be
* made here.
*
* Please note: GPIO index in all functions use the `GPIO_x` index number and not
* physcal MCU index or pin # for abstraction from the chip.
*******************************************************************************/

// Enable GPIO peripheral - setting to false will disable (not initialized at boot)
#define HW_USE_GPIO true

// define the total number of GPIO pins used here
// this should match the highest `GPIO_x` defines below plus one
#define GPIO_COUNT 4

// GPIO pin IDs - note that 'GPIO_x' can correspond to any unused GPIO# on the MCU
#define GPIO_0_MCU_ID 10
#define GPIO_1_MCU_ID 11
#define GPIO_2_MCU_ID 12
#define GPIO_3_MCU_ID 13
// add the above pin IDs to this ordered list for expansion in the settings array structure
#define GPIO_MCU_IDS    GPIO_0_MCU_ID, \
                        GPIO_1_MCU_ID, \
                        GPIO_2_MCU_ID, \
                        GPIO_3_MCU_ID

// set mode as input (GPIO_IN) or output (GPIO_OUT)
#define GPIO_0_DIRECTION GPIO_OUT
#define GPIO_1_DIRECTION GPIO_OUT
#define GPIO_2_DIRECTION GPIO_IN
#define GPIO_3_DIRECTION GPIO_IN
// add the above pin directions to this ordered list for expansion in the settings array structure
#define GPIO_DIRECTIONS GPIO_0_DIRECTION, \
                        GPIO_1_DIRECTION, \
                        GPIO_2_DIRECTION, \
                        GPIO_3_DIRECTION

// define GPIO pull state possible values, don't touch these!!
#define GPIO_PULL_DISABLED 0 // don't touch
#define GPIO_PULL_UP       1 // don't touch
#define GPIO_PULL_DOWN     2 // don't touch
// set pull mode of pins (GPIO_PULL_UP, GPIO_PULL_DOWN, GPIO_PULL_DISABLED)
// note this only applies to inputs, outputs on RP2040 are push/pull
#define GPIO_0_PULL GPIO_PULL_DISABLED
#define GPIO_1_PULL GPIO_PULL_DISABLED
#define GPIO_2_PULL GPIO_PULL_DISABLED
#define GPIO_3_PULL GPIO_PULL_DISABLED
// add the above GPIO pulls to this ordered list for expansion in the settings array structure
#define GPIO_PULLS  GPIO_0_PULL, \
                    GPIO_1_PULL, \
                    GPIO_2_PULL, \
                    GPIO_3_PULL

// for inputs, should an ISR be associated to a state change (true/false)
// note that only one ISR can be configured per processor core
// currently the only ISR defined is 'gpio_process' - see function prototype below (can be customized)
#define GPIO_0_IRQ_EN false
#define GPIO_1_IRQ_EN false
#define GPIO_2_IRQ_EN false
#define GPIO_3_IRQ_EN false
// add the above IRQ enables to this ordered list for expansion in the settings array structure
#define GPIO_IRQS   GPIO_0_IRQ_EN, \
                    GPIO_1_IRQ_EN, \
                    GPIO_2_IRQ_EN, \
                    GPIO_3_IRQ_EN

// GPIO settings structure, used to initialize all GPIO pins and interract with
// them at runtime.
typedef struct gpio_settings_t {
    uint    gpio_mcu_id[GPIO_COUNT];
    bool    gpio_direction[GPIO_COUNT];
    uint8_t gpio_pull[GPIO_COUNT];
    bool    gpio_irq_en[GPIO_COUNT];
} gpio_settings_t;

// global GPIO settings stored here to be accessed in other files
extern struct gpio_settings_t gpio_settings;

// GPIO event structure, used to hold GPIO IRQ events
typedef struct gpio_event_t {
    uint gpio;
    uint32_t event_mask;
    uint64_t timestamp;
} gpio_event_t;

// global GPIO event holder for last IRQ event that occured
extern struct gpio_event_t gpio_event;

// global GPIO mutex
extern SemaphoreHandle_t gpio_mutex;

/**
* @brief GPIO process ISR callback.
*
* Generic Interrupt Service Routine to process a GPIO state change IRQ.
* This is a simple routine that identifies the change that happened and updates
* a global variable. Note that on RP2040, only one ISR can be associated to a
* GPIO IRQ event per core.
*
* @param gpio which GPIO caused this interrupt
* @param event_mask which events caused this interrupt. See pico SDK gpio.h 'gpio_irq_level' for details
*
* @return nothing
*/
void gpio_process(uint gpio, uint32_t event_mask);

/**
* @brief Initialize all used GPIO.
*
* Initialize all of the GPIO pins defined in the gpio_settings configuration
* structure, based on the configuration definitions above.
*
* @param none
*
* @return nothing
*/
void gpio_init_all(void);

/**
* @brief Get the value of a single GPIO pin.
*
* Gets the value (0, 1) of a single GPIO pin defined by its ID - GPIO_x_MCU_ID
*
* @param gpio_id GPIO ID x given by GPIO_x_MCU_ID definition
*
* @return GPIO pin state (0, 1)
*/
bool gpio_read_single(uint gpio_id);

/**
* @brief Set the value of a single GPIO pin.
*
* Sets the value (0, 1) of a single GPIO pin defined by its ID - GPIO_x_MCU_ID
*
* @param gpio_id GPIO ID x given by GPIO_x_MCU_ID definition
*
* @return nothing
*/
void gpio_write_single(uint gpio_id, bool value);

/**
* @brief Read values of all used GPIO pins.
*
* Reads the current state of all GPIO pins that are defined as GPIO_IN.
* The return value is a 32-bit number with LSB representing GPIO_0 as defined
* in the settings above, and MSB representing GPIO_31 (uint_32 return value
* supports up to 32 GPIO pins). Any bits representing pins that are set to
* GPIO_OUT are ignored. The global gpio_settings structure can be used to
* iterate through used GPIO inputs and associate them with physical pin number.
*
* @param none
*
* @return 32-bit number containing up to 32 GPIO pin state values
*/
uint32_t gpio_read_all(void);

/**
* @brief Write the values of all used GPIO pins.
*
* Writes the value of all GPIO pins that are defined as GPIO_OUT, given a 32-bit
* number representing up to 32 individual GPIO pins. Individual bits representing
* single GPIO outputs can be shifted in based on the pin indexes defined in
* the global gpio_settings structure. Any bits that correspond to pins that are
* set to GPIO_IN are ignored.
*
* @param gpio_states 32-bit number containing up to 32 GPIO values to write
*
* @return nothing
*/
void gpio_write_all(uint32_t gpio_states);


/************************
 * CLI UART
*************************/

// CLI UART Settings
#define UART_ID_CLI         uart0
#define UART_BAUD_RATE_CLI  115200
#define UART_DATA_BITS_CLI  8
#define UART_STOP_BITS_CLI  1
#define UART_PARITY_CLI     UART_PARITY_NONE
#define UART_TX_PIN_CLI     0
#define UART_RX_PIN_CLI     1

// global CLI UART mutex
extern SemaphoreHandle_t cli_uart_mutex;

/**
* @brief Initialize the CLI UART.
*
* Initialization routine for the UART peripheral used for the
* Command Line Interface using the settings defined above.
*
* @param none
*
* @return nothing
*/
void cli_uart_init(void);

/**
* @brief Write character to CLI UART.
*
* Write a single character to the UART used for the Command Line Interface.
* This function is used by Microshell for all CLI output.
*
* @param tx_char character to write
*
* @return 1 upon success, 0 upon failure
*/
int cli_uart_putc(char tx_char);

/**
* @brief Read character from CLI UART.
*
* Read a single character from the UART used for the Command Line Interface in
* a non-blocking fashion. This function is used by Microshell for all CLI input.
*
* @param none
*
* @return Character read from UART
*/
char cli_uart_getc(void);

/**
* @brief Print a string to the CLI UART.
*
* Print an entire string to the UART used for the Command Line Interface. This
* function can be used in the CLI outside of Microshell to print debug & status
* messages. The function will block until the entire string is written.
*
* @param print_string pointer to the string to print
*
* @return nothing
*/
void cli_uart_puts(const char *print_string);


/************************
 * Auxilliary UART
*************************/

// Enable Aux UART - setting to false will disable (not initialized at boot)
#define HW_USE_AUX_UART true

// AUX UART Settings
#define UART_ID_AUX             uart1
#define UART_BAUD_RATE_AUX      115200
#define UART_DATA_BITS_AUX      8
#define UART_STOP_BITS_AUX      1
#define UART_PARITY_AUX         UART_PARITY_NONE
#define UART_TX_PIN_AUX         8
#define UART_RX_PIN_AUX         9
#define UART_RX_FIFO_SIZE_AUX   32 // from RP2040 datasheet (hardware limited)

// global Aux UART mutex
extern SemaphoreHandle_t aux_uart_mutex;

/**
* @brief Initialize the AUX UART.
*
* Initialization routine for the UART peripheral used for the
* auxilliary serial interface using the settings defined above.
*
* @param none
*
* @return nothing
*/
void aux_uart_init(void);

/**
* @brief Write bytes to AUX UART.
*
* Write one or more bytes to the UART used for the auxilliary serial interface.
*
* @param tx_data pointer to the data to write
* @param tx_len  length in bytes of data to write
*
* @return 1 upon success, 0 upon failure
*/
int aux_uart_write(uint8_t *tx_data, size_t tx_len);

/**
* @brief Read bytes from AUX UART.
*
* Read any bytes available on the auxilliary UART in a non-blocking fashion,
* until no more bytes are immediately available or rx_len has been reached.
*
* @param rx_data pointer to the data buffer to hold the bytes received
* @param rx_len  maximum number of bytes to read (buffer size)
*
* @return number of bytes received
*/
int aux_uart_read(uint8_t *rx_data, size_t rx_len);


/************************
 * I2C0 Master
*************************/

// Enable I2C0 peripheral - setting to false will disable (not initialized at boot)
#define HW_USE_I2C0 true

// I2C0 Settings
#define I2C0_ID            i2c0
#define I2C0_FREQ_KHZ      100
#define I2C0_SDA_PIN       20
#define I2C0_SCL_PIN       21

// global I2C0 mutex
extern SemaphoreHandle_t i2c0_mutex;

/**
* @brief Initialize I2C bus 0.
*
* Initialization routine for the I2C0 peripheral using the settings defined
* above.
*
* @param none
*
* @return nothing
*/
void i2c0_init(void);

/**
* @brief Write bytes onto I2C0 bus.
*
* Put byte(s) onto I2C bus 0 given a specific device address. It is assumed this
* is a blocking function.
*
* @param addr 7-bit address (0-127) of target device on I2C bus
* @param write_data pointer to the byte array to write onto the bus
* @param length number of bytes to write onto the bus
*
* @return number of bytes written, or -1 if no response
*/
int i2c0_write(uint8_t addr, const uint8_t *write_data, size_t length);

/**
* @brief Read bytes off of I2C0 bus.
*
* Read byte(s) from a given device adress on I2C bus 0. It is assumed this is a
* blocking function.
*
* @param addr 7-bit address (0-127) of target device on I2C bus
* @param read_data pointer to the byte array to store the data read from the bus
* @param length number of bytes to read from the bus
*
* @return number of bytes read, or -1 if no response
*/
int i2c0_read(uint8_t addr, uint8_t *read_data, size_t length);


/************************
 * SPI0 Master
*************************/

// Enable SPI0 peripheral - setting to false will disable (not initialized at boot)
#define HW_USE_SPI0 true

// SPI0 Settings
#define SPI0_ID             spi0
#define SPI0_FREQ_KHZ       500
#define SPI0_MISO_PIN       4
#define SPI0_MOSI_PIN       3
#define SPI0_CLK_PIN        2
#define SPI0_CS_PIN_DEFAULT 5

// Target/peripheral device settings -
// used to define a target device by the chip select it is attached to, and the
// register address that can be interrogated for the expected device ID to
// validate communication
#define SPI0_TARGET_DEV_0_CS     SPI0_CS_PIN_DEFAULT
#define SPI0_TARGET_DEV_0_ID     0x60
#define SPI0_TARGET_DEV_0_ID_REG 0xD0
// more TARGET_DEV entries and associated chip selects can be added to expand this list

// global SPI0 mutex
extern SemaphoreHandle_t spi0_mutex;

/**
* @brief Initialize SPI bus 0.
*
* Initialization routine for the SPI0 peripheral using the settings defined
* above.
*
* @param none
*
* @return nothing
*/
void spi0_init(void);

/**
* @brief Write a byte into a SPI0 register address.
*
* Writes a single byte to a target device identified by the chip select pin, into
* the register at the given address.
*
* @param cs_pin    master pin number for the chip select line connected to target device
* @param reg_addr  8-bit register address to write to
* @param data_byte single-byte of data to write into target register    
*
* @return 1 if success, 0 if failure
*/
int spi0_write_register(uint8_t cs_pin, uint8_t reg_addr, uint8_t data_byte);

/**
* @brief Read byte(s) from a SPI0 register address.
*
* Reads one or more bytes from a target device identified by the chip select pin,
* from the given register address. Multiple successive registers can be read in
* series by specifying the read length. This is a blocking read.
*
* @param cs_pin   master pin number for the chip select line connected to target device
* @param reg_addr 8-bit register address to read from
* @param read_buf pointer to the byte array to store the data read from target
* @param len      number of successive register locations to read from
*
* @return number of bytes read
*/
int spi0_read_registers(uint8_t cs_pin, uint8_t reg_addr, uint8_t *read_buf, uint16_t len);


/************************
 * On-board LED
*************************/

// Enable onboard LED - setting to false will disable (not initialized at boot)
// note - if using a Pico W board, the onboard LED is connected to the CYW43 radio -
// this would need the WiFi chip initialized, which we do not currently do. Thus
// the onboard LED will be default disabled in that case.
#define HW_USE_ONBOARD_LED true

// On-board LED Settings
#define PIN_NO_ONBOARD_LED 25 // does not apply to Pico W board (connected to CYW43)

/**
* @brief Initialization routine for the onboard LED.
*
* This function initializes the GPIO used as the driver for the onboard LED.
*
* @param none
*
* @return nothing
*/
void onboard_led_init(void);

/**
* @brief Sets the state of the onboard LED.
*
* This function sets the value of the onboard LED GPIO to enable (turn ON) or
* disable (turn OFF) the LED.
*
* @param led_state 0 (disabled) or 1 (enabled)
*
* @return nothing
*/
void onboard_led_set(bool led_state);

/**
* @brief Gets the state of the onboard LED.
*
* This function gets the value of the onboard LED GPIO to determine if the LED
* is enabled (ON) or disabled (OFF).
*
* @param none
*
* @return 0 (disabled) or 1 (enabled)
*/
bool onboard_led_get(void);


/************************
 * Watchdog Timer
*************************/

// Watchdog Timer Settings
#define WATCHDOG_DELAY_MS        5000 // default watchdog timer delay
#define WATCHDOG_DELAY_REBOOT_MS 100  // delay for reboot function

/**
* @brief Enables the watchdog timer.
*
* This function enables the hardware watchdog timer at the specified interval.
* On RP2040, there will be a standard soft reboot when watchdog timer expires.
*
* @param delay_ms millisecond interval for the watchdog before reboot
*
* @return nothing
*/
void watchdog_en(uint32_t delay_ms);

/**
* @brief Disables the watchdog timer.
*
* This function disables the hardware watchdog timer.
*
* @param none
*
* @return nothing
*/
void watchdog_dis(void);

/**
* @brief Reset the watchdog timer.
*
* Resets the watchdog timer to the start value given when the timer was started
* with watchdog_en(). One must kick the dog before this timer expires to prevent
* the watchdog expiration action (default to soft reboot). Be assured that this
* source code in no way condones kicking dogs or any other life form for that
* matter.
*
* @param none
*
* @return nothing `
*/
void watchdog_kick(void);

/**
* @brief Force a reboot.
*
* Forces a reboot of the MCU by waiting for watchdog timeout.
*
* @param none
*
* @return nothing
*/
void force_watchdog_reboot(void);


/************************
 * Chip Reset
*************************/

// Reset reason types
typedef enum {POWERON,   // normal power-on reset
              GLITCH,    // power supply glitch reset
              BROWNOUT,  // brownout reset
              WATCHDOG,  // watchdog timeout reset
              FORCED,    // application-requested reset
              PIN,       // external pin-toggled reset
              DOUBLETAP, // double-tap external pin reset
              DEBUGGER,  // attached debugger reset
              UNKNOWN    // reset reason could not be detected
             } reset_reason_t;

// Global reset reason type - set at boot
extern reset_reason_t last_reset_reason;

/**
* @brief Get the reset reason.
*
* This function will return the reason for the last system reset, i.e. POR,
* watchdog, program-requested, reset pin, or debugger.
*
* @param none
*
* @return reset reason given by reset_reason_t enum
*/
reset_reason_t get_reset_reason(void);

/**
* @brief Get the reset reason as a string.
*
* This function is a wrapper for get_reset_reason() which returns a human-readable
* string that can be used for printing to the CLI.
*
* @param reset_reason reset reason given by reset_reason_t enum, provided by get_reset_reason()
*
* @return pointer to the reset reason string
*/
char* get_reset_reason_string(reset_reason_t reset_reason);

/**
* @brief Reset chip to the bootloader.
*
* Forces a reset of the MCU into the bootloader so that firmware can be updated.
* On RP2040 this is the built-in DFU mode.
*
* @param none
*
* @return nothing
*/
void reset_to_bootloader(void);


/************************
 * System Clocks/Timers
*************************/

/**
* @brief Get the system clock frequency.
*
* Returns the current frequency of the internal system clock that the MCU has
* determined it is using, in Hz.
*
* @param none
*
* @return system clock frequency, Hz
*/
uint32_t get_sys_clk_hz(void);

/**
* @brief Get system timer value.
*
* Returns the current system timer value in microseconds. Size uint64 means the
* timer will not roll over for 584,942 years. When using the function to print
* the time, use snprintf with 14 bytes reserved for ASCII timer value to get ~3 years
* before rollover (13 bytes for ~0.3 years, 15 bytes for ~30 years, etc.)
*
* @param none
*
* @return timer value, microseconds
*/
uint64_t get_time_us(void);

/**
* @brief Wait for the specified amount of microseconds.
*
* Waits (delays) execution of further code for the specified number of microseconds.
* Similar to running many NOOPs to waste cycles for a specified amount of time.
*
* @param delay_us number of microseconds to delay
*
* @return nothing
*/
void wait_here_us(uint64_t delay_us);


/************************
 * System MCU Cores
*************************/

/**
* @brief Get the core that is executing.
*
* Returns the current core number that this function was executed on when
* running on a multi-core MCU.
*
* @param none
*
* @return core number
*/
uint8_t get_core(void);


/************************
 * Chip Versions
*************************/

/**
* @brief Get the silicon version number.
*
* Returns the integer value version number of the MCU silicon.
*
* @param none
*
* @return version number
*/
uint8_t get_chip_version(void);

/**
* @brief Get the ROM version number.
*
* Returns the integer value version number of the MCU's ROM.
*
* @param none
*
* @return version number
*/
uint8_t get_rom_version(void);


/************************
 * Chip Registers
*************************/

/**
* @brief Read a chip register.
*
* Reads and returns a single 32-bit register from the MCU's memory-mapped
* register space.
*
* @param reg_addr address of the register to read
*
* @return 32-bit value of the register
*/
uint32_t read_chip_register(uint32_t reg_addr);


/************************
 * Onboard Flash
*************************/

// Enable onboard flash - setting to false will disable program access to flash
#define HW_USE_ONBOARD_FLASH true

// Onboard Flash Settings
#define FLASH0_FS_SIZE   (256 * 1024)       // size of the 'flash0' filesystem (intended for littlefs to manage)
#define PATHNAME_MAX_LEN  32                // maximum string length of path+filename on the filesystem
#define FILE_SIZE_MAX     BUF_OUT_SIZE      // maximum size of a single file in bytes - set to shell output buffer size so entire files can be dumped
#define FLASH0_BLOCK_SIZE FLASH_SECTOR_SIZE // "block" size in littlefs terms is "sector" size in RP2040 terms

// Onboard flash usage detail structure
typedef struct flash_usage_t {
    int flash_total_size;
    int program_used_size;
    int fs_reserved_size;
    int flash_free_size;
} flash_usage_t;

// global onboard flash mutex
extern SemaphoreHandle_t onboard_flash_mutex;

/**
* @brief Initialize onboard flash memory.
*
* This function enables usage of the onboard flash memory (this is the same NVM
* used for program storage). Note that all this function does at this time is
* create the mutex used for onboard flash access, since flash I/O is already
* technically available at runtime.
*
* @param none
*
* @return nothing
*/
void onboard_flash_init(void);

/**
* @brief Read from onboard flash.
*
* This function reads a given address range of "onboard" flash (the same NVM
* used for program storage). Argument structure conforms to littlefs, however
* the function may also be used standalone.
*
* @param c littlefs configuration, can populate with a dummy if using outside of littlefs
* @param block address of the flash block (sector) to read from
* @param offset byte offset within the block of read start
* @param buffer pointer to the read buffer
* @param size size of data to read
*
* @return 0 upon success
*/
int onboard_flash_read(const struct lfs_config *c, uint32_t block, uint32_t offset, void* buffer, uint32_t size);

/**
* @brief Write to onboard flash.
*
* This function writes data to "onboard" flash (the same NVM used for program
* storage). Note that the block of flash written to must have been previously
* erased. Argument structure conforms to littlefs, however the function may also
* be used standalone.
*
* @param c littlefs configuration, can populate with a dummy if using outside of littlefs
* @param block address of the flash block (sector) to write to
* @param offset byte offset within the block of write start
* @param buffer pointer to the buffer containing data to write
* @param size size of data to write
*
* @return 0 upon success
*/
int onboard_flash_write(const struct lfs_config *c, uint32_t block, uint32_t offset, const void* buffer, uint32_t size);

/**
* @brief Erase onboard flash.
*
* This function erases a block (sector) of "onboard" flash (the same NVM used
* for program storage). Argument structure conforms to littlefs, however the
* function may also be used standalone.
*
* @param c littlefs configuration, can populate with a dummy if using outside of littlefs
* @param block address of the flash block (sector) to erase
*
* @return 0 upon success
*/
int onboard_flash_erase(const struct lfs_config *c, uint32_t block);

/**
* @brief Sync the state of onboard flash.
*
* This is a dummy sync function since littlefs will hardfault if the sync
* function pointer is null.
*
* @param c littlefs configuration
*
* @return 0 upon success
*/
int onboard_flash_sync(const struct lfs_config *c);

/**
* @brief Get flash usage detail.
*
* Determine the application binary flash memory usage, total flash size, and
* remaining "free" space available (minus any other NVM consumers such as
* filesystem, etc)
*
* @param none
*
* @return structure containing flash usage detail
*/
flash_usage_t onboard_flash_usage(void);


/*************************************
 * ADC - Analog-to-Digital Coverters
**************************************/

// Enable ADC peripheral - setting to false will disable all ADCs (not initialized at boot)
#define HW_USE_ADC true

// ADC channel settings
#define ADC0_GPIO 26 // this is ADC0
#define ADC1_GPIO 27 // this is ADC1
#define ADC2_GPIO 28 // this is ADC2

// individual ADC initialization
#define ADC0_INIT true
#define ADC1_INIT false
#define ADC2_INIT false

// ACD conversion
#define ADC_CONV_FACT (3.3f / (1 << 12)) // 3.3V reference, 12-bit resolution

// global ADC mutex
extern SemaphoreHandle_t adc_mutex;

/**
* @brief Initialize the ADC peripheral(s).
*
* Initializes the ADC peripheral. On chips with multiple ADC peripherals, each
* will be initialized seperately within this function. Note that even if an MCU
* has multiple ADC input pins, they may share a single ADC block (as is the case
* on RP2040).
*
* @param none
*
* @return nothing
*/
void adcs_init(void);

/**
* @brief Read ADC value.
*
* Read the result of the analog-to-digital conversion on the given analog input
* channel, converted to a floating-point voltage value. Note that these channels
* may share the same ADC block internal to the MCU. Here, "channel" refers to
* a specific GPIO/pin given in the settings above.
*
* @param adc_channel channel number of the ADC to read
*
* @return value of the analog-to-digital conversion (voltage reading)
*/
float read_adc(int adc_channel);


/************************
 * USB (TinyUSB) CDC
*************************/

// Enable USB controller - setting to false will disable (not initialized at boot)
// note that if CLI uses USB, it will override this
#define HW_USE_USB true

// set USB mode (device)
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

// number of composite CDC interfaces and data buffer sizes
#define CFG_TUD_CDC 1
#define CFG_TUD_CDC_RX_BUFSIZE 64
#define CFG_TUD_CDC_TX_BUFSIZE 64
// make sure buffer size is within bounds of uint16_t
#if CFG_TUD_CDC_RX_BUFSIZE > 65535 || CFG_TUD_CDC_TX_BUFSIZE > 65535
    #error "USB buffer size exceeds bounds"
#endif

// CDC interface ID for CLI over USB (if used)
#define CDC_ID_CLI 0

// USB interface data structure
typedef struct usb_iface_t {
    uint8_t  iface_id;                          // composite interface ID
    bool     is_conn;                           // connection flag
    uint8_t  rx_buffer[CFG_TUD_CDC_RX_BUFSIZE]; // receive buffer (bytes from host)
    uint8_t  tx_buffer[CFG_TUD_CDC_TX_BUFSIZE]; // transmit buffer (bytes to host)
    uint16_t rx_pos;                            // current position in rx buffer
    uint16_t tx_pos;                            // current position in tx buffer
} usb_iface_t;

// global USB mutex
extern SemaphoreHandle_t usb_mutex;

/**
* @brief Initialize the serial number used for USB device ID.
*
* Retrieves the unique ID (serial number) of the device and sets it as the USB
* device serial no. reported to the bus.
*
* @param none
*
* @return nothing
*/
void usb_serialno_init(void);

/**
* @brief Initialize the USB device.
*
* Initializes the USB controller in device mode using TinyUSB.
*
* @param none
*
* @return nothing
*/
void usb_device_init(void);

/**
* @brief Read bytes out of the USB device buffer.
*
* This function will check for an active connection on the given interface,
* check if there are bytes available in the device buffer, and then read out
* (copy) any available bytes into the USB interface structure instance. The
* rx_pos value in usb_iface will then be updated to reflect how many bytes are
* currently available in the rx buffer.
*
* @param usb_iface USB interface structure for a given composite device ID
*
* @return nothing
*/
void usb_read_bytes(struct usb_iface_t *usb_iface);

/**
* @brief Write bytes to the USB device buffer.
*
* This function will copy bytes from the given USB interface structure into the
* device buffer (if tx_pos > 0 indicating bytes available to be written),
* and send the bytes to the host.
*
* @param usb_iface USB interface structure for a given composite device ID
*
* @return nothing
*/
void usb_write_bytes(struct usb_iface_t *usb_iface);

/**
* @brief Write character to CLI over USB.
*
* Write a single character to the USB CDC ID used for the Command Line Interface.
* This function is used by Microshell for all CLI output if CLI_USE_USB=1 at
* compile time.
*
* @param tx_char character to write to USB
*
* @return 1 upon success, 0 upon failure
*/
int cli_usb_putc(char tx_char);

/**
* @brief Read character from CLI over USB.
*
* Read a single character from the USB CDC ID used for the Command Line Interface
* in a non-blocking fashion. This function is used by Microshell for all CLI
* input if CLI_USE_USB=1 at compile time.
*
* @param none
*
* @return Character read from USB
*/
char cli_usb_getc(void);


/**************************
 * Wireless (CYW43 WiFi/BT)
***************************/

// Enable CYW43 wireless module - setting to false will disable (not initialized at boot)
// This only applies to boards with the wireless module, otherwise it is ignored.
// note that if using the onboard LED, the CYW43 must be enabled (LED is controlled by CYW43)
#define HW_USE_CYW43 true

// Make sure CYW43 is enabled if using onboard LED
#if HAS_CYW43 == true && HW_USE_CYW43 == false && HW_USE_ONBOARD_LED == true
#error "CYW43 must be enabled in hardware_config.h if using onboard LED"
#endif

// Note: due to complexity, all WiFi-specific typedefs, options, function
// declarations and documentation are contained in the hw_wifi.h file.


#endif /* HARDWARE_CONFIG_H */