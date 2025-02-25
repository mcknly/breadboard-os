/******************************************************************************
 * @file motd.h
 *
 * @brief contains the ASCII string (or art!) for MOTD (message of the day)
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

#ifndef MOTD_H
#define MOTD_H

#include <microshell.h>
#include "version.h"

// stringify helper - use xstr() to convert #define to a usable string
#define str(s) # s
#define xstr(s) str(s)

// place motd message in motd_ascii character array pointer
// thanks to:
// great ASCII art generator  - https://github.com/TheZoraiz/ascii-image-converter
// great ASCII font generator - https://patorjk.com/software/taag
//
// remember...any forward slashes need to be escaped by an extra /
// also make sure there are no quotes in the art, replace them with some other char
//
// the image below relies on "braille" characters (UTF-8/Unicode) and requires a suitable
// terminal font to render properly, specifically support for characters U+2800..U+28FF.
const char *motd_ascii =
"\r\n"
USH_SHELL_FONT_COLOR_YELLOW
" ⣿⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⡠⠤⢤⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⢀⠤⠐⠒⠊⢉⣉⣉⣉⣉⠁⠀⠀⠀⡀⠀⠠⠄⣉⠑⠒⠤⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⢀⠊⢁⡠⢔⣚⣭⡵⠒⠀⠈⠶⠭⠑⢒⡦⢄⠉⠲⡢⢄⠉⠲⡢⣄⡈⠑⠢⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⡐⢁⠔⠕⠘⠻⢛⣻⣿⣀⠀⠀⠀⠀⠀⠀⠀⠑⠅⡢⡈⠑⢍⢄⠈⢄⢮⣕⡢⣀⠑⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⢰⠀⠆⣶⣄⣰⣿⣿⣿⡿⠿⠁⠀⠀⠀⠀⢀⣀⣀⣀⣈⠊⢕⢄⠑⠂⠈⣂⣀⣀⣀⣉⣢⣌⠑⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠸⡀⠑⢝⠿⣿⣿⣿⣷⣦⣀⣀⣀⡀⢸⣿⣿⠿⢟⡫⠭⠒⠒⢉⣉⠤⠤⠤⠤⠤⠤⠤⢄⣈⣁⠀⠑⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⢀⣀⣐⡱⠀⠀⠑⠪⣛⠿⣿⣿⣿⣿⣿⣿⡮⡫⠕⢉⣡⠤⣖⣒⣭⣵⣶⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⡍⡂⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⠒⢉⣉⣁⠤⠤⠤⢤⠀⢄⣀⠀⠉⠒⠭⡻⢿⡿⢋⠊⡠⢒⣵⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣿⡇⡃⢠⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⠀⡈⠢⡲⣿⣿⣿⢸⠀⢰⢰⣕⣒⠄⡀⠈⠑⠊⠁⢐⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣎⠛⠟⠔⠀⡊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⡄⢨⠢⡈⠪⡻⣿⣜⢀⠐⠬⡻⢿⣿⣮⣕⢄⡀⠀⢸⢸⣿⣿⣿⣿⠿⠿⠿⠿⠿⠿⠛⠛⠃⡀⣸⣿⣿⣾⠞⠂⠐⠤⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣔⢄⠑⢌⠢⡈⢊⠻⣷⣅⡢⣈⠑⠭⡻⢿⣷⡎⡆⠨⡘⣅⡀⠁⠀⢀⣠⣤⠤⠴⢶⣒⣛⡫⠭⠭⠕⠒⠒⠊⢀⣉⣀⣀⣀⣈⡉⠒⠢⢄⣀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠁⠑⠄⠑⢌⠢⡑⢌⠻⣿⣶⣕⠤⡈⠑⠍⡣⢣⠀⠇⡿⡫⠭⠭⠒⠒⠒⢉⣉⣁⡠⠤⠤⢔⣒⣒⣺⣭⣭⣵⣶⣶⣶⣶⣶⣮⣭⣖⡦⢄⣉⡉⠒⠒⠦⡀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠪⡢⡀⠠⡈⢂⠑⢜⢿⣿⣿⣮⣒⢄⡈⠑⠀⢉⠊⠀⠶⣒⣒⠭⢭⡽⣶⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⢗⣒⡬⠭⠓⠂⢰⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠈⢂⠈⠢⡑⢄⠑⢝⢿⣿⣿⣷⣮⡲⢄⣀⡀⠀⠀⠀⠀⠉⠑⡙⢿⣿⣿⣿⣿⣿⣿⣿⠿⣛⣛⣛⠭⠭⠭⠭⠗⠒⠒⠋⢉⣁⣀⠤⠔⠒⠀⡸⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠐⠕⢄⠀⢄⠑⢄⠑⠝⢿⣿⣿⣿⣷⣶⣔⢄⡀⠀⠀⠀⠀⠀⠉⠒⠺⠭⠕⠒⠒⠒⠉⠤⠤⠤⡀⠒⠒⣒⠒⠊⣭⡉⠁⢀⣀⡠⠤⠊⡠⣈⠑⠤⡻ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠁⡀⠑⢄⠑⡀⠢⡙⣿⣿⣿⣿⣿⣷⣮⣖⡢⢄⠀⠀⠀⠒⠒⠒⠒⠒⠉⠍⢄⡑⠪⠥⠤⠀⠑⠻⠵⠒⠒⠋⢉⣁⣠⠤⠔⠊⢞⣒⣑⠢⠈ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠈⠪⡢⡀⠠⡨⡢⡈⠪⡻⣿⣿⣿⣿⣿⣿⣿⣷⣕⢄⡀⠑⠒⠒⠒⠒⠒⠒⠊⢉⣀⠠⠔⣒⣒⣒⡲⠦⠭⠭⠓⠒⠒⠊⢉⣉⣁⣠⠤⠤⠀ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢈⢄⠈⠪⡪⡢⡈⠪⡻⣿⣿⣿⣿⣿⡿⠿⠷⢎⣉⣑⡲⠶⠶⠟⠛⠛⠋⢉⣉⣉⣀⠤⠤⠤⠔⠒⠒⠒⠉⠉⠉⠁⠀⠰⠾⢟⡐⢀ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠕⢄⠐⢌⢈⠢⡈⠪⠭⠝⠒⠒⠊⢉⣉⣁⡠⠤⠤⢔⣒⣒⣪⣭⣭⣽⣶⣶⠦⠤⢄⣀⣀⡠⠤⠤⠴⠒⠒⠒⠉⢉⣉⣁⡠⢬ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡡⡀⠑⠙⢨⠀⠀⡒⣪⣭⣭⣵⣶⣶⣾⡿⠿⢟⣛⣛⡭⠭⠭⠒⠒⠚⢉⣉⣁⡠⠤⠤⠤⠒⠒⠒⠉⠉⠉⠁⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠪⡢⡈⠪⡀⠀⢑⣛⣛⡯⠭⠽⠒⠒⠊⣉⣉⡠⠤⠤⠖⠒⠒⠉⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠪⠢⢄⣀⣀⡠⠤⠤⠖⠒⠚⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿ \r\n"
" ⣿⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣿ \r\n"
USH_SHELL_FONT_STYLE_RESET;

// the bbos ascii header is seperate from MOTD so we can choose to print just one.
// it doesn't look right here because of the extra '\' escapes, but will magically
// look great in the terminal.
const char *bbos_header_ascii =
USH_SHELL_FONT_COLOR_YELLOW
"                                   ___          ___                     \r\n"
"          _____       _____       /  /\\        /  /\\                  \r\n"
"         /  /::\\     /  /::\\     /  /::\\      /  /:/_                \r\n"
"        /  /:/\\:\\   /  /:/\\:\\   /  /:/\\:\\    /  /:/ /\\           \r\n"
"       /  /:/~/::\\ /  /:/~/::\\ /  /:/  \\:\\  /  /:/ /::\\            \r\n"
"      /__/:/ /:/\\:/__/:/ /:/\\:/__/:/ \\__\\:\\/__/:/ /:/\\:\\         \r\n"
"      \\  \\:\\/:/~/:|  \\:\\/:/~/:|  \\:\\ /  /:/\\  \\:\\/:/~/:/      \r\n"
"       \\  \\::/ /:/ \\  \\::/ /:/ \\  \\:\\  /:/  \\  \\::/ /:/        \r\n"
"        \\  \\:\\/:/   \\  \\:\\/:/   \\  \\:\\/:/    \\__\\/ /:/       \r\n"
"         \\  \\::/     \\  \\::/     \\  \\::/       /__/:/             \r\n"
"          \\__\\/       \\__\\/       \\__\\/        \\__\\/  "
xstr(BBOS_VERSION_MAJOR) "." xstr(BBOS_VERSION_MINOR) // add version number to header
" " // add an extra space that will be replaced by BBOS_VERSION_MOD
"\r\n"
"\r\n"
USH_SHELL_FONT_COLOR_MAGENTA
USH_SHELL_FONT_STYLE_BOLD
"                [["
USH_SHELL_FONT_COLOR_CYAN
" Welcome to BreadboardOS! "
USH_SHELL_FONT_COLOR_MAGENTA
"]]\r\n"
"\r\n"
USH_SHELL_FONT_STYLE_RESET;

#endif /* MOTD_H */