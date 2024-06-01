/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#include <bbos.h>
#include <microshell.h>
#include "shell.h"

extern struct portmux portmux[];
extern int num_portmux;
extern struct periph_info peripherals[];
extern int num_peripherals;
extern struct board_pinouts board_pinouts[];
extern int picosdk_to_periph[][2];

#define HELP_PINOUTS \
"show all pinouts:\n" \
"   pinouts\n"


static void dump_pinmap(void)
{
    char buf[128], funcsel[16];
    int prev_gpio, *pin_funcs, i;

    pin_funcs = malloc(num_portmux * sizeof(int));
    for (i = 0; i < num_portmux; i++)
        pin_funcs[i] = -1;

    prev_gpio = -1;
    for (i = 0; i < num_portmux; i++) {
        if (MCU_TO_BOARD_PIN(portmux[i].mcu_pin) == 0) {
            /* Pin is not broken out on this board. */
            continue;
        }
        if (prev_gpio != portmux[i].gpio) {
            if (prev_gpio != -1) {
                uart_puts(CUR_UART, "\n");
            }
            pin_funcs[portmux[i].mcu_pin] = gpio_get_function(portmux[i].gpio);
            snprintf(buf, 128, "%3d: ", MCU_TO_BOARD_PIN(portmux[i].mcu_pin));
            uart_puts(CUR_UART, buf);
        }
        if (peripherals[portmux[i].pin_info.periph].func_type == picosdk_to_periph[pin_funcs[portmux[i].mcu_pin]][1])
            snprintf(funcsel, 16, "%s*", portmux[i].pin_info.name);
        else
            strncpy(funcsel, portmux[i].pin_info.name, 16);
        snprintf(buf, 128, "%-9s ", funcsel);
        uart_puts(CUR_UART, buf);
        prev_gpio = portmux[i].gpio;
    }
    uart_puts(CUR_UART, "\n");

    free(pin_funcs);
}

void cmd_pinouts(struct ush_object *self, struct ush_file_descriptor const *file,
        int argc, char *argv[])
{
    struct periph_info *per;
    int *pins, i;
    long pin;
    char buf[128], *anum, *err;

    if (argc == 1) {
        dump_pinmap();
    } else if (argc > 2 && !strcmp(argv[1], "enable")) {
        per = NULL;
        for (i = 0; i < num_peripherals; i++) {
            if (!strcmp(argv[2], peripherals[i].name)) {
                per = &peripherals[i];
                break;
            }
        }
        if (!per) {
            snprintf(buf, 128, "unknown peripheral %s\n", argv[2]);
            uart_puts(CUR_UART, buf);
            return;
        }

        pins = malloc((MAX_PINSET_SIZE + 1) * sizeof(int));
        pins[0] = -1;
        if (argc > 4 && !strcmp(argv[3], "pins")) {
            pins[0] = -1;
            for (i = 0; i < MAX_PINSET_SIZE; i++) {
                anum = strtok(i == 0 ? argv[4] : NULL, ",");
                if (anum == NULL)
                    break;
                if (*anum == 0) {
                    uart_puts(CUR_UART, "invalid pins\n");
                    return;
                }
                pin = strtol(anum, &err, 10);
                if (err == anum || *err != 0) {
                    snprintf(buf, 128, "invalid pin '%s'\n", anum);
                    uart_puts(CUR_UART, buf);
                    return;
                }
                pins[i] = pin;
            }
            pins[i] = -1;
        }

        free(pins);
    } else {
        uart_puts(CUR_UART, "wrong arguments\n");
    }
}


static const struct ush_file_descriptor cmd_files[] = {
    {
        .name = "pinouts",
        .description = "manage pinouts",
        .help = HELP_PINOUTS,
        .exec = cmd_pinouts,
    },
};

// cmd commands handler
static struct ush_node_object cmd;

void pinouts_commands_add(void)
{
    // add custom commands
    ush_commands_add(&ush, &cmd, cmd_files, sizeof(cmd_files) / sizeof(cmd_files[0]));
}
