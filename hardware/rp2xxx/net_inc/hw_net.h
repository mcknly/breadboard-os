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

#endif /* HW_NET_H */