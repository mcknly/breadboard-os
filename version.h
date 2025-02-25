/******************************************************************************
 * @file version.h
 *
 * @brief contains the version number of the base BreadboardOS project
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

#ifndef VERSION_H
#define VERSION_H

// stringify helper - use xstr() to convert #define to a usable string
#define str(s) # s
#define xstr(s) str(s)

// maintain BreadboardOS version so it can be tracked along with custom project
#define BBOS_NAME          "breadboard-os" // do not change
#define BBOS_VERSION_MAJOR 0               // changed only for major release update
#define BBOS_VERSION_MINOR 3               // changed only for minor release update

// global "modified version" variable will hold '+' if on a branch other than main
// this is checked and defined in the cli_service startup routine
extern char BBOS_VERSION_MOD;

// custom project name and version is defined in top level CMakeLists.txt
// look for PROJ_NAME and PROJ_VER

#endif /* VERSION_H */