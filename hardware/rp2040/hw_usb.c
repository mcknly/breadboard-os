/******************************************************************************
 * @file hw_usb.c
 *
 * @brief Functions for configuring and implementing a USB device using the
 *        TinyUSB library. The implementation of these functions may be
 *        MCU-specific and will need to be updated if ported to a new hardware
 *        family.
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
#include "hardware/flash.h"
#include "tusb.h"
#include "semphr.h"

// global USB mutex
SemaphoreHandle_t usb_mutex;


/************************
 * USB Descriptor Setup
*************************/

#define DESC_STR_MAX 20

#define USBD_VID 0x2E8A // Vendor: Raspberry Pi
#define USBD_PID 0x000A // Product: Raspberry Pi Pico CDC

#define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN * CFG_TUD_CDC)
#define USBD_MAX_POWER_MA 500

#define USBD_ITF_CDC_0 0
#define USBD_ITF_MAX 2

#define USBD_CDC_0_EP_CMD 0x81
#define USBD_CDC_0_EP_OUT 0x01
#define USBD_CDC_0_EP_IN 0x82

#define USBD_CDC_CMD_MAX_SIZE 8
#define USBD_CDC_IN_OUT_MAX_SIZE 64

#define USBD_STR_0 0x00
#define USBD_STR_MANUF 0x01
#define USBD_STR_PRODUCT 0x02
#define USBD_STR_SERIAL 0x03
#define USBD_STR_SERIAL_LEN 17
#define USBD_STR_CDC 0x04

static const tusb_desc_device_t usbd_desc_device = {
	.bLength = sizeof(tusb_desc_device_t),
	.bDescriptorType = TUSB_DESC_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = TUSB_CLASS_MISC,
	.bDeviceSubClass = MISC_SUBCLASS_COMMON,
	.bDeviceProtocol = MISC_PROTOCOL_IAD,
	.bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
	.idVendor = USBD_VID,
	.idProduct = USBD_PID,
	.bcdDevice = 0x0100,
	.iManufacturer = USBD_STR_MANUF,
	.iProduct = USBD_STR_PRODUCT,
	.iSerialNumber = USBD_STR_SERIAL,
	.bNumConfigurations = 1,
};

// endpoint descriptors, can add additional TUD_CDC_DESCRIPTOR to array for more
// endpoints in the composite device
static const uint8_t usbd_desc_cfg[USBD_DESC_LEN] = {
	TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_MAX, USBD_STR_0, USBD_DESC_LEN,
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),

	TUD_CDC_DESCRIPTOR(USBD_ITF_CDC_0, USBD_STR_CDC, USBD_CDC_0_EP_CMD,
		USBD_CDC_CMD_MAX_SIZE, USBD_CDC_0_EP_OUT, USBD_CDC_0_EP_IN,
		USBD_CDC_IN_OUT_MAX_SIZE),
};

static char usb_serialno[USBD_STR_SERIAL_LEN] = "000000000000";

static const char *const usbd_desc_str[] = {
	[USBD_STR_MANUF] = "Raspberry Pi",
	[USBD_STR_PRODUCT] = "Pico",
	[USBD_STR_SERIAL] = usb_serialno,
	[USBD_STR_CDC] = "Board CDC",
};


/*******************************
 * TinyUSB Descriptor Callbacks
********************************/

// TinyUSB callback to provide the device profile
const uint8_t *tud_descriptor_device_cb(void) {
	return (const uint8_t *) &usbd_desc_device;
}

// TinyUSB callback to provide configuration, interface, and endpoint profiles
const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
	return usbd_desc_cfg;
}

// TinyUSB callback to provide the device strings in UTF8
const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
	static uint16_t desc_str[DESC_STR_MAX];
	uint8_t len;

	if (index == 0) {
		desc_str[1] = 0x0409;
		len = 1;
	} else {
		const char *str;
		char serial[USBD_STR_SERIAL_LEN];

		if (index >= sizeof(usbd_desc_str) / sizeof(usbd_desc_str[0]))
			return NULL;

		str = usbd_desc_str[index];
		for (len = 0; len < DESC_STR_MAX - 1 && str[len]; ++len)
			desc_str[1 + len] = str[len];
	}

	desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);

	return desc_str;
}

void usb_serialno_init(void) {
	uint8_t id[8];

    // Get unique board ID from the Pico flash (programmed during manufacturing)
	flash_get_unique_id(id);

	snprintf(usb_serialno,
	         USBD_STR_SERIAL_LEN,
			 "%02X%02X%02X%02X%02X%02X%02X%02X",
		     id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
}

void usb_device_init(void) {
	// create USB mutex
    usb_mutex = xSemaphoreCreateMutex();

	// generate unique USB device serial no. from RPi Pico unique flash ID
	usb_serialno_init();
	
	// initialize the TinyUSB device
	tusb_init();
}

void usb_read_bytes(struct usb_iface_t *usb_iface) {
	// check if connected
	if (usb_iface->is_conn) {
		// check if bytes available to read
		uint16_t len = tud_cdc_n_available(usb_iface->iface_id);
		// read bytes if available and less than size left in buffer
		if (len > 0 && (usb_iface->rx_pos + len) < CFG_TUD_CDC_RX_BUFSIZE) {
			if(xSemaphoreTake(usb_mutex, 10) == pdTRUE) { // attempt to acquire mutex so we can read
				usb_iface->rx_pos += tud_cdc_n_read(usb_iface->iface_id, usb_iface->rx_buffer + usb_iface->rx_pos, len);
				xSemaphoreGive(usb_mutex);
			}
		}
	}
}

void usb_write_bytes(struct usb_iface_t *usb_iface) {
	// check if connected and there are bytes to send
	if (usb_iface->is_conn && usb_iface->tx_pos > 0) {
		if(xSemaphoreTake(usb_mutex, 10) == pdTRUE) { // attempt to acquire mutex so we can write
			uint16_t count = tud_cdc_n_write(usb_iface->iface_id, usb_iface->tx_buffer, usb_iface->tx_pos);
			if (count > 0) {
				tud_cdc_n_write_flush(usb_iface->iface_id);
				if (count < usb_iface->tx_pos) { // if all bytes in the write buffer haven't been written, shift them up
					memmove(usb_iface->tx_buffer, &usb_iface->tx_buffer[count], usb_iface->tx_pos - count);
					usb_iface->tx_pos -= count;
				}
				else {
					usb_iface->tx_pos = 0;
				}
			}
			xSemaphoreGive(usb_mutex);
		}
	}
}

int cli_usb_putc(char tx_char) {
	int status = 0;

	// check if connected
	if (tud_cdc_n_connected(CDC_ID_CLI)) {
		// write single char (byte) if mutex is available
		if(xSemaphoreTake(usb_mutex, 10) == pdTRUE) {
			if (tud_cdc_n_write_char(CDC_ID_CLI, tx_char) == 1) {
				tud_cdc_n_write_flush(CDC_ID_CLI);
				status = 1;
			}
			xSemaphoreGive(usb_mutex);
		}
	}

	return status;
}

char cli_usb_getc(void) {
	char readchar = NOCHAR;

	// check if connected and there are bytes available
	if (tud_cdc_n_connected(CDC_ID_CLI) && tud_cdc_n_available(CDC_ID_CLI) > 0) {
		// read single char (byte) if mutex is available
		if(xSemaphoreTake(usb_mutex, 10) == pdTRUE) {
			readchar = tud_cdc_n_read_char(CDC_ID_CLI);
			xSemaphoreGive(usb_mutex);
		}
	}

	return readchar;
}