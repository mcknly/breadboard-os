/******************************************************************************
 * @file hw_net.h
 *
 * @brief Settings, definitions, and function prototypes for various network
 *        stacks, much of it tied to lwIP. Hardware and SDK-specific
 *        implementation resides within these functions.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 02-06-2025
 *
 * @copyright Copyright (c) 2025 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef HW_NET_H
#define HW_NET_H

#include <stdint.h>
#include "lwip/arch.h"


/**
* @brief Initialize mDNS
*
* This function intializes the lwIP mDNS stack. Once running, the device will
* be discoverable on the local network via the assigned hostname (set by the
* HOSTNAME plus BOARD variables in project.cmake). The resulting mDNS host will
* be accessible at '<HOSTNAME>-<BOARD>.local'.
*
* @param none
*
* @return nothing
*/
void net_mdns_init(void);

/**
* @brief Initialize the httpd stack
*
* This function is called to initialize the lwIP httpd (web server) stack. Once
* running, the web server is available on the standard http port (80) and can
* serve dynamic content via SSI. Must already be connected to a network and have
* an assigned IP.
*
* @param none
*
* @return nothing
*/
void net_httpd_stack_init(void);

/**
* @brief lwIP httpd CGI handler
*
* This function is called by the lwIP httpd stack when a CGI request is made
* (i.e. the key/value pair in the URI after the '?' character). This function
* must be assigned to a path in the httpd_cgi_paths array. Note that multiple
* CGI handler functions can be defined and assigned to different paths.
*
* @param tCGIHandler Params given by lwIP typedef tCGIHandler. See lwip/apps/httpd.h
*
* @return pointer to URI path string returned to the browser
*/
static const char *httpd_cgi_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

/**
* @brief lwIP httpd SSI handler
*
* This function is called by the lwIP httpd stack when an SSI request is made
* (i.e. the server found a SSI tag in the html file). This function contains
* logic which matches the iIndex tag index number defined by the httpd_ssi_tags
* array. The function will return the string to be inserted into the HTML file
* in place of the tag. Note that there is only one SSI handler function assigned
* to the lwIP httpd stack.
*
* @param tSSIHandler Params given by lwIP typedef tSSIHandler. See lwip/apps/httpd.h
*
* @return number of characters written in place of the SSI tag
*/
u16_t httpd_ssi_handler(int iIndex, char *pcInsert, int iInsertLen, uint16_t current_tag_part, uint16_t *next_tag_part);

#endif /* HW_NET_H */