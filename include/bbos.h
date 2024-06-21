/* SPDX-License-Identifier: MIT */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ARRAY_AND_SIZE(x) (x), ARRAY_SIZE(x)

/* temp */
#define CUR_UART uart0

/* Convert MCU pin to board pin. MCU pins are indexed on 0, board pins
 * start at 1. */
#define MCU_TO_BOARD_PIN(x) (board_pinouts[x-1].board_pin)

enum func_type {
    FUNC_TYPE_GPIO,
    FUNC_TYPE_SPI,
    FUNC_TYPE_UART,
    FUNC_TYPE_I2C,
    FUNC_TYPE_PWM,
    FUNC_TYPE_PIO,
    FUNC_TYPE_CLOCK,
    FUNC_TYPE_USB,
};

/* Peripheral info and pin list */
struct periph_info {
    int periph;
    enum func_type func_type;
    char *name;
    char **pins;
    int num_pins;
};

/* MCU datasheet pin function to semantic function mapping */
struct pin_info {
    char *name;
    int periph;
};

/* MCU pin to function mapping */
struct portmux {
    int mcu_pin;
    int gpio;
    struct pin_info pin_info;
};

/* MCU pinout to board pin/marking mapping. Entries correspond to MCU
 * pin numbers, counting from 1. */
struct board_pinouts {
    /* Board numbering starting with 1, or 0 if not broken out */
    int board_pin;
    /* Board silkscreen marking, or NULL */
    char *marking;
};

/* Board-specific info and pinouts */
struct board_info {
    char *board_name;   /* Corresponds to the BOARD build variable */
    char *full_name;
    struct board_pinouts *pinouts;
};

//struct extdriver {
//    char *name;
//    char *description;
//    void * (*init) (int periph);
//    int (*shutdown) (void);
//};


