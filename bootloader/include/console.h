#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>
#include <stdint.h>

#include "boot_types.h"

// ANSI escape code blackmagic
#define CONSOLE_CLEAR        "\033[2J\033[H"
#define CONSOLE_COLOR_RESET  "\033[0m"
#define CONSOLE_BOLD         "\033[1m"
#define CONSOLE_COLOR_CYAN   "\033[36m"
#define CONSOLE_COLOR_GREEN  "\033[32m"
#define CONSOLE_COLOR_YELLOW "\033[33m"
#define CONSOLE_COLOR_RED    "\033[31m"

void console_init(void);
void console_print_banner(void);
void console_print_menu(void);
void console_clear_screen(void);

#endif 
