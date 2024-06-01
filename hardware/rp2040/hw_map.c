/* SPDX-License-Identifier: MIT */

#include <stdlib.h>
#include <bbos.h>

#include <hardware/gpio.h>

/* Peripherals on this MCU */
enum periph {
    PERIPH_GPIO,
    PERIPH_SPI0,
    PERIPH_SPI1,
    PERIPH_UART0,
    PERIPH_UART1,
    PERIPH_I2C0,
    PERIPH_I2C1,
    PERIPH_PWM0,
    PERIPH_PWM1,
    PERIPH_PWM2,
    PERIPH_PWM3,
    PERIPH_PWM4,
    PERIPH_PWM5,
    PERIPH_PWM6,
    PERIPH_PWM7,
    PERIPH_PIO0,
    PERIPH_PIO1,
    PERIPH_CLOCK,
    PERIPH_USB,
};

char *periph_funcs_spi0[] = { "SPI0_RX", "SPI0_TX", "SPI0_SCK", "SPI0_CSn", };
char *periph_funcs_spi1[] = { "SPI1_RX", "SPI1_TX", "SPI1_SCK", "SPI1_CSn", };
char *periph_funcs_uart0[] = { "UART0_RX", "UART0_TX", };
char *periph_funcs_uart1[] = { "UART1_RX", "UART1_TX", };
char *periph_funcs_i2c0[] = { "I2C0_SDA", "I2C0_SCL", };
char *periph_funcs_i2c1[] = { "I2C1_SDA", "I2C1_SCL", };
char *periph_funcs_pwm0[] = { "PWM0_A", "PWM0_B" };
char *periph_funcs_pwm1[] = { "PWM1_A", "PWM1_B" };
char *periph_funcs_pwm2[] = { "PWM2_A", "PWM2_B" };
char *periph_funcs_pwm3[] = { "PWM3_A", "PWM3_B" };
char *periph_funcs_pwm4[] = { "PWM4_A", "PWM4_B" };
char *periph_funcs_pwm5[] = { "PWM5_A", "PWM5_B" };
char *periph_funcs_pwm6[] = { "PWM6_A", "PWM6_B" };
char *periph_funcs_pwm7[] = { "PWM7_A", "PWM7_B" };
char *periph_funcs_clock[] = { "CLk_IN1", "CLK_OUT1", "CLK_OUT2", "CLK_OUT3" };
char *periph_funcs_usb[] = { "USB_OVCUR_DET", "USB_VBUS_DET", "USB_VBUS_EN", };

struct periph_info peripherals[] = {
    { PERIPH_GPIO, FUNC_TYPE_GPIO, "gpio"},
    { PERIPH_SPI0, FUNC_TYPE_SPI, "spi0", ARRAY_AND_SIZE(periph_funcs_spi0) },
    { PERIPH_SPI1, FUNC_TYPE_SPI, "spi1", ARRAY_AND_SIZE(periph_funcs_spi1) },
    { PERIPH_UART0, FUNC_TYPE_UART, "uart0", ARRAY_AND_SIZE(periph_funcs_uart0) },
    { PERIPH_UART1, FUNC_TYPE_UART, "uart1", ARRAY_AND_SIZE(periph_funcs_uart1) },
    { PERIPH_I2C0, FUNC_TYPE_I2C, "i2c0", ARRAY_AND_SIZE(periph_funcs_i2c0) },
    { PERIPH_I2C1, FUNC_TYPE_I2C, "i2c1", ARRAY_AND_SIZE(periph_funcs_i2c1) },
    { PERIPH_PWM0, FUNC_TYPE_PWM, "pwm0", ARRAY_AND_SIZE(periph_funcs_pwm0) },
    { PERIPH_PWM1, FUNC_TYPE_PWM, "pwm1", ARRAY_AND_SIZE(periph_funcs_pwm1) },
    { PERIPH_PWM2, FUNC_TYPE_PWM, "pwm2", ARRAY_AND_SIZE(periph_funcs_pwm2) },
    { PERIPH_PWM3, FUNC_TYPE_PWM, "pwm3", ARRAY_AND_SIZE(periph_funcs_pwm3) },
    { PERIPH_PWM4, FUNC_TYPE_PWM, "pwm4", ARRAY_AND_SIZE(periph_funcs_pwm4) },
    { PERIPH_PWM5, FUNC_TYPE_PWM, "pwm5", ARRAY_AND_SIZE(periph_funcs_pwm5) },
    { PERIPH_PWM6, FUNC_TYPE_PWM, "pwm6", ARRAY_AND_SIZE(periph_funcs_pwm6) },
    { PERIPH_PWM7, FUNC_TYPE_PWM, "pwm7", ARRAY_AND_SIZE(periph_funcs_pwm7) },
    { PERIPH_PIO0, FUNC_TYPE_PIO, "pio0" },
    { PERIPH_PIO1, FUNC_TYPE_PIO, "pio1" },
    { PERIPH_CLOCK, FUNC_TYPE_CLOCK, "clock", ARRAY_AND_SIZE(periph_funcs_clock) },
    { PERIPH_USB, FUNC_TYPE_USB, "usb", ARRAY_AND_SIZE(periph_funcs_usb) },
};
int num_peripherals = ARRAY_SIZE(peripherals);

/* Keep sorted by MCU pin. */
struct portmux portmux[] = {
    { .mcu_pin=2, .gpio=0, .pin_info={ .name="GPIO0", .periph=PERIPH_GPIO } },
    { 2, 0, { "SPI0_RX", PERIPH_SPI0 } },
    { 2, 0, { "UART0_TX", PERIPH_UART0 } },
    { 2, 0, { "I2C0_SDA", PERIPH_I2C0 } },
    { 2, 0, { "PWM0_A", PERIPH_PWM0 } },
    { 2, 0, { "PIO0", PERIPH_PIO0 } },
    { 2, 0, { "PIO1", PERIPH_PIO1 } },
    { 2, 0, { "USB_OVCUR_DET", PERIPH_USB } },

    { 3, 1, { "GPIO1", PERIPH_GPIO }, },
    { 3, 1, { "SPI0_CSn", PERIPH_SPI0, }, },
    { 3, 1, { "UART0_RX", PERIPH_UART0, }, },
    { 3, 1, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 2, 1, { "PWM0_B", PERIPH_PWM0 } },
    { 3, 1, { "PIO0", PERIPH_PIO0 } },
    { 3, 1, { "PIO1", PERIPH_PIO1 } },
    { 3, 1, { "USB_VBUS_DET", PERIPH_USB } },

    { 4, 2, { "GPIO2", PERIPH_GPIO }, },
    { 4, 2, { "SPI0_SCK", PERIPH_SPI0, }, },
    { 4, 2, { "UART0_CTS", PERIPH_UART0, }, },
    { 4, 2, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 4, 2, { "PWM1_A", PERIPH_PWM1, }, },
    { 4, 2, { "PIO0", PERIPH_PIO0 } },
    { 4, 2, { "PIO1", PERIPH_PIO1 } },
    { 4, 2, { "USB_VBUS_EN", PERIPH_USB } },

    { 5, 3, { "GPIO3", PERIPH_GPIO }, },
    { 5, 3, { "SPI0_TX", PERIPH_SPI0, }, },
    { 5, 3, { "UART0_RTS", PERIPH_UART0, }, },
    { 5, 3, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 5, 3, { "PWM1_B", PERIPH_PWM1 }, },
    { 5, 3, { "PIO0", PERIPH_PIO0, }, },
    { 5, 3, { "PIO1", PERIPH_PIO1, }, },
    { 5, 3, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 6, 4, { "GPIO4", PERIPH_GPIO }, },
    { 6, 4, { "SPI0_RX", PERIPH_SPI0, }, },
    { 6, 4, { "UART1_TX", PERIPH_UART1, }, },
    { 6, 4, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 6, 4, { "PWM2_A", PERIPH_PWM2, }, },
    { 6, 4, { "PIO0", PERIPH_PIO0, }, },
    { 6, 4, { "PIO1", PERIPH_PIO1, }, },
    { 6, 4, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 7, 5, { "GPIO5", PERIPH_GPIO }, },
    { 7, 5, { "SPI0_CSn", PERIPH_SPI0, }, },
    { 7, 5, { "UART1_RX", PERIPH_UART1, }, },
    { 7, 5, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 7, 5, { "PWM2_B", PERIPH_PWM2, }, },
    { 7, 5, { "PIO0", PERIPH_PIO0, }, },
    { 7, 5, { "PIO1", PERIPH_PIO1, }, },
    { 7, 5, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 8, 6, { "GPIO6", PERIPH_GPIO }, },
    { 8, 6, { "SPI0_SCK", PERIPH_SPI0, }, },
    { 8, 6, { "UART1_CTS", PERIPH_UART1, }, },
    { 8, 6, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 8, 6, { "PWM3_A", PERIPH_PWM3, }, },
    { 8, 6, { "PIO0", PERIPH_PIO0, }, },
    { 8, 6, { "PIO1", PERIPH_PIO1, }, },
    { 8, 6, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 9, 7, { "GPIO7", PERIPH_GPIO }, },
    { 9, 7, { "SPI0_TX", PERIPH_SPI0, }, },
    { 9, 7, { "UART1_RTS", PERIPH_UART1, }, },
    { 9, 7, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 9, 7, { "PWM3_B", PERIPH_PWM3, }, },
    { 9, 7, { "PIO0", PERIPH_PIO0, }, },
    { 9, 7, { "PIO1", PERIPH_PIO1, }, },
    { 9, 7, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 11, 8, { "GPIO8", PERIPH_GPIO }, },
    { 11, 8, { "SPI1_RX", PERIPH_SPI1, }, },
    { 11, 8, { "UART1_TX", PERIPH_UART1, }, },
    { 11, 8, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 11, 8, { "PWM4_A", PERIPH_PWM4, }, },
    { 11, 8, { "PIO0", PERIPH_PIO0, }, },
    { 11, 8, { "PIO1", PERIPH_PIO1, }, },
    { 11, 8, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 12, 9, { "GPIO9", PERIPH_GPIO }, },
    { 12, 9, { "SPI1_CSn", PERIPH_SPI1, }, },
    { 12, 9, { "UART1_RX", PERIPH_UART1, }, },
    { 12, 9, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 12, 9, { "PWM4_B", PERIPH_PWM4, }, },
    { 12, 9, { "PIO0", PERIPH_PIO0, }, },
    { 12, 9, { "PIO1", PERIPH_PIO1, }, },
    { 12, 9, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 13, 10, { "GPIO10", PERIPH_GPIO }, },
    { 13, 10, { "SPI1_SCK", PERIPH_SPI1, }, },
    { 13, 10, { "UART1_CTS", PERIPH_UART1, }, },
    { 13, 10, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 13, 10, { "PWM5_A", PERIPH_PWM5, }, },
    { 13, 10, { "PIO0", PERIPH_PIO0, }, },
    { 13, 10, { "PIO1", PERIPH_PIO1, }, },
    { 13, 10, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 14, 11, { "GPIO11", PERIPH_GPIO }, },
    { 14, 11, { "SPI1_TX", PERIPH_SPI1, }, },
    { 14, 11, { "UART1_RTS", PERIPH_UART1, }, },
    { 14, 11, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 14, 11, { "PWM5_B", PERIPH_PWM5, }, },
    { 14, 11, { "PIO0", PERIPH_PIO0, }, },
    { 14, 11, { "PIO1", PERIPH_PIO1, }, },
    { 14, 11, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 15, 12, { "GPIO12", PERIPH_GPIO }, },
    { 15, 12, { "SPI1_RX", PERIPH_SPI1, }, },
    { 15, 12, { "UART0_TX", PERIPH_UART0, }, },
    { 15, 12, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 15, 12, { "PWM6_A", PERIPH_PWM6, }, },
    { 15, 12, { "PIO0", PERIPH_PIO0, }, },
    { 15, 12, { "PIO1", PERIPH_PIO1, }, },
    { 15, 12, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 16, 13, { "GPIO13", PERIPH_GPIO }, },
    { 16, 13, { "SPI1_CSn", PERIPH_SPI1, }, },
    { 16, 13, { "UART0_RX", PERIPH_UART0, }, },
    { 16, 13, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 16, 13, { "PWM6_B", PERIPH_PWM6, }, },
    { 16, 13, { "PIO0", PERIPH_PIO0, }, },
    { 16, 13, { "PIO1", PERIPH_PIO1, }, },
    { 16, 13, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 17, 14, { "GPIO14", PERIPH_GPIO }, },
    { 17, 14, { "SPI1_SCK", PERIPH_SPI1, }, },
    { 17, 14, { "UART0_CTS", PERIPH_UART0, }, },
    { 17, 14, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 17, 14, { "PWM7_A", PERIPH_PWM7, }, },
    { 17, 14, { "PIO0", PERIPH_PIO0, }, },
    { 17, 14, { "PIO1", PERIPH_PIO1, }, },
    { 17, 14, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 18, 15, { "GPIO15", PERIPH_GPIO }, },
    { 18, 15, { "SPI1_TX", PERIPH_SPI1, }, },
    { 18, 15, { "UART0_RTS", PERIPH_UART0, }, },
    { 18, 15, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 18, 15, { "PWM7_B", PERIPH_PWM7, }, },
    { 18, 15, { "PIO0", PERIPH_PIO0, }, },
    { 18, 15, { "PIO1", PERIPH_PIO1, }, },
    { 18, 15, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 27, 16, { "GPIO16", PERIPH_GPIO }, },
    { 27, 16, { "SPI0_RX", PERIPH_SPI0, }, },
    { 27, 16, { "UART0_TX", PERIPH_UART0, }, },
    { 27, 16, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 27, 16, { "PWM0_A", PERIPH_PWM0, }, },
    { 27, 16, { "PIO0", PERIPH_PIO0, }, },
    { 27, 16, { "PIO1", PERIPH_PIO1, }, },
    { 27, 16, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 28, 17, { "GPIO17", PERIPH_GPIO }, },
    { 28, 17, { "SPI0_CSn", PERIPH_SPI0, }, },
    { 28, 17, { "UART0_RX", PERIPH_UART0, }, },
    { 28, 17, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 28, 17, { "PWM0_B", PERIPH_PWM0, }, },
    { 28, 17, { "PIO0", PERIPH_PIO0, }, },
    { 28, 17, { "PIO1", PERIPH_PIO1, }, },
    { 28, 17, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 29, 18, { "GPIO18", PERIPH_GPIO }, },
    { 29, 18, { "SPI0_SCK", PERIPH_SPI0, }, },
    { 29, 18, { "UART0_CTS", PERIPH_UART0, }, },
    { 29, 18, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 29, 18, { "PWM1_A", PERIPH_PWM1, }, },
    { 29, 18, { "PIO0", PERIPH_PIO0, }, },
    { 29, 18, { "PIO1", PERIPH_PIO1, }, },
    { 29, 18, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 30, 19, { "GPIO19", PERIPH_GPIO }, },
    { 30, 19, { "SPI0_TX", PERIPH_SPI0, }, },
    { 30, 19, { "UART0_RTS", PERIPH_UART0, }, },
    { 30, 19, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 30, 19, { "PWM1_B", PERIPH_PWM1, }, },
    { 30, 19, { "PIO0", PERIPH_PIO0, }, },
    { 30, 19, { "PIO1", PERIPH_PIO1, }, },
    { 30, 19, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 31, 20, { "GPIO20", PERIPH_GPIO }, },
    { 31, 20, { "SPI0_RX", PERIPH_SPI0, }, },
    { 31, 20, { "UART1_TX", PERIPH_UART1, }, },
    { 31, 20, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 31, 20, { "PWM2_A", PERIPH_PWM2, }, },
    { 31, 20, { "PIO0", PERIPH_PIO0, }, },
    { 31, 20, { "PIO1", PERIPH_PIO1, }, },
    { 31, 20, { "CLK_IN0", PERIPH_CLOCK, }, },
    { 31, 20, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 32, 21, { "GPIO21", PERIPH_GPIO }, },
    { 32, 21, { "SPI0_CSn", PERIPH_SPI0, }, },
    { 32, 21, { "UART1_RX", PERIPH_UART1, }, },
    { 32, 21, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 32, 21, { "PWM2_B", PERIPH_PWM2, }, },
    { 32, 21, { "PIO0", PERIPH_PIO0, }, },
    { 32, 21, { "PIO1", PERIPH_PIO1, }, },
    { 32, 21, { "CLK_OUT0", PERIPH_CLOCK, }, },
    { 32, 21, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 34, 22, { "GPIO22", PERIPH_GPIO }, },
    { 34, 22, { "SPI0_SCK", PERIPH_SPI0, }, },
    { 34, 22, { "UART1_CTS", PERIPH_UART1, }, },
    { 34, 22, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 34, 22, { "PWM3_A", PERIPH_PWM3, }, },
    { 34, 22, { "PIO0", PERIPH_PIO0, }, },
    { 34, 22, { "PIO1", PERIPH_PIO1, }, },
    { 34, 22, { "CLK_IN1", PERIPH_CLOCK, }, },
    { 34, 22, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 35, 23, { "GPIO23", PERIPH_GPIO }, },
    { 35, 23, { "SPI0_TX", PERIPH_SPI0, }, },
    { 35, 23, { "UART1_RTS", PERIPH_UART1, }, },
    { 35, 23, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 35, 23, { "PWM3_B", PERIPH_PWM3, }, },
    { 35, 23, { "PIO0", PERIPH_PIO0, }, },
    { 35, 23, { "PIO1", PERIPH_PIO1, }, },
    { 35, 23, { "CLK_OUT1", PERIPH_CLOCK, }, },
    { 35, 23, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 36, 24, { "GPIO24", PERIPH_GPIO }, },
    { 36, 24, { "SPI1_RX", PERIPH_SPI1, }, },
    { 36, 24, { "UART1_TX", PERIPH_UART1, }, },
    { 36, 24, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 36, 24, { "PWM4_A", PERIPH_PWM4, }, },
    { 36, 24, { "PIO0", PERIPH_PIO0, }, },
    { 36, 24, { "PIO1", PERIPH_PIO1, }, },
    { 36, 24, { "CLK_OUT2", PERIPH_CLOCK, }, },
    { 36, 24, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 37, 25, { "GPIO25", PERIPH_GPIO }, },
    { 37, 25, { "SPI1_CSn", PERIPH_SPI1, }, },
    { 37, 25, { "UART1_RX", PERIPH_UART1, }, },
    { 37, 25, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 37, 25, { "PWM4_B", PERIPH_PWM4, }, },
    { 37, 25, { "PIO0", PERIPH_PIO0, }, },
    { 37, 25, { "PIO1", PERIPH_PIO1, }, },
    { 37, 25, { "CLK_OUT3", PERIPH_CLOCK, }, },
    { 37, 25, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 38, 26, { "GPIO26", PERIPH_GPIO }, },
    { 38, 26, { "SPI1_SCK", PERIPH_SPI1, }, },
    { 38, 26, { "UART1_CTS", PERIPH_UART1, }, },
    { 38, 26, { "I2C1_SDA", PERIPH_I2C1, }, },
    { 38, 26, { "PWM5_A", PERIPH_PWM5, }, },
    { 38, 26, { "PIO0", PERIPH_PIO0, }, },
    { 38, 26, { "PIO1", PERIPH_PIO1, }, },
    { 38, 26, { "USB_VBUS_EN", PERIPH_USB, }, },

    { 39, 27, { "GPIO27", PERIPH_GPIO }, },
    { 39, 27, { "SPI1_TX", PERIPH_SPI1, }, },
    { 39, 27, { "UART1_RTS", PERIPH_UART1, }, },
    { 39, 27, { "I2C1_SCL", PERIPH_I2C1, }, },
    { 39, 27, { "PWM5_B", PERIPH_PWM5, }, },
    { 39, 27, { "PIO0", PERIPH_PIO0, }, },
    { 39, 27, { "PIO1", PERIPH_PIO1, }, },
    { 39, 27, { "USB_OVCUR_DET", PERIPH_USB, }, },

    { 40, 28, { "GPIO28", PERIPH_GPIO }, },
    { 40, 28, { "SPI1_RX", PERIPH_SPI1, }, },
    { 40, 28, { "UART0_TX", PERIPH_UART0, }, },
    { 40, 28, { "I2C0_SDA", PERIPH_I2C0, }, },
    { 40, 28, { "PWM6_A", PERIPH_PWM6, }, },
    { 40, 28, { "PIO0", PERIPH_PIO0, }, },
    { 40, 28, { "PIO1", PERIPH_PIO1, }, },
    { 40, 28, { "USB_VBUS_DET", PERIPH_USB, }, },

    { 41, 29, { "GPIO29", PERIPH_GPIO }, },
    { 41, 29, { "SPI1_CSn", PERIPH_SPI1, }, },
    { 41, 29, { "UART0_RX", PERIPH_UART0, }, },
    { 41, 29, { "I2C0_SCL", PERIPH_I2C0, }, },
    { 41, 29, { "PWM6_B", PERIPH_PWM6, }, },
    { 41, 29, { "PIO0", PERIPH_PIO0, }, },
    { 41, 29, { "PIO1", PERIPH_PIO1, }, },
    { 41, 29, { "USB_VBUS_EN", PERIPH_USB, }, },
};
int num_portmux = ARRAY_SIZE(portmux);

/* Pico SDK gpio_(get|set)_function values mapped to internal port type,
 * indexed according to the SDK's enum gpio_function. */
int picosdk_to_periph[][2] = {
    { GPIO_FUNC_XIP, -1 },
    { GPIO_FUNC_SPI, FUNC_TYPE_SPI },
    { GPIO_FUNC_UART, FUNC_TYPE_UART },
    { GPIO_FUNC_I2C, FUNC_TYPE_I2C },
    { GPIO_FUNC_PWM, FUNC_TYPE_PWM },
    { GPIO_FUNC_SIO, FUNC_TYPE_GPIO },
    { GPIO_FUNC_PIO0, FUNC_TYPE_PIO },
    { GPIO_FUNC_PIO1, FUNC_TYPE_PIO },
    { GPIO_FUNC_GPCK, FUNC_TYPE_CLOCK },
    { GPIO_FUNC_USB, FUNC_TYPE_USB },
    /* TODO too sparse, waste of space */
    [0x1f] = { GPIO_FUNC_NULL, -1 },
};

/* BBOS always deals with MCU pin numbers internally, since it needs to
 * control the hardware directly. However these don't match the board
 * pin numbers on the silkscreen, which is what the user sees. This
 * maps MCU pins to board pins. */
#if BOARD_IS_RPI_PICO
struct board_pinouts board_pinouts[] = {
    // board pin (or 0), marking
    /* 1 */
    { 0 },      /* IOVDD */
    { 1, "1" }, /* GPIO0 */
    { 2, "2" }, /* GPIO1 */
    { 4 },      /* GPIO2 */
    { 5 },      /* GPIO3 */
    { 6 },      /* GPIO4 */
    { 7 },      /* GPIO5 */
    { 9 },      /* GPIO6 */
    { 10 },     /* GPIO7 */
    { 0 },      /* IOVDD */
    { 11 },     /* GPIO8 */
    { 12 },     /* GPIO9 */
    { 14 },     /* GPIO10 */
    { 15 },     /* GPIO11 */
    /* 15 */
    { 16 },     /* GPIO12 */
    { 17 },     /* GPIO13 */
    { 19 },     /* GPIO14 */
    { 20 },     /* GPIO15 */
    { 0 },      /* TESTEN */
    { 0 },      /* XIN */
    { 0 },      /* XOUT */
    { 0 },      /* IOVDD */
    { 0 },      /* DVDD */
    { 0 },      /* SWCLK */
    { 0 },      /* SWDIO */
    { 0 },      /* RUN */
    { 21 },     /* GPIO16 */
    { 22 },     /* GPIO17 */
    /* 29 */
    { 24 },     /* GPIO18 */
    { 25 },     /* GPIO19 */
    { 26 },     /* GPIO20 */
    { 27 },     /* GPIO21 */
    { 0 },      /* IOVDD */
    { 29 },     /* GPIO22 */
    { 0 },      /* GPIO23 */
    { 0 },      /* GPIO24 */
    { 0 },      /* GPIO25 */
    { 31 },     /* GPIO26 */
    { 32 },     /* GPIO27 */
    { 34 },     /* GPIO28 */
    { 0 },      /* GPIO29 */
    { 0 },      /* IOVDD */
    /* 43 */
    { 0 },      /* ADC_AVDD */
    { 0 },      /* VREG_VIN */
    { 0 },      /* VREG_VOUT */
    { 0 },      /* USB_DM */
    { 0 },      /* USB_DP */
    { 0 },      /* USB_VDD */
    { 0 },      /* IOVDD */
    { 0 },      /* DVDD */
    { 0 },      /* QSPI_SD3 */
    { 0 },      /* QSPI_SCLK */
    { 0 },      /* QSPI_SD0 */
    { 0 },      /* QSPI_SD2 */
    { 0 },      /* QSPI_SD1 */
    { 0 },      /* QSPI_SS_N */

};
#endif
