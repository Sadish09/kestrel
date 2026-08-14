#include "../include/console.h"
#include "../include/uart_protocol.h"

void console_init(void){
    (void)uart_protocol_init();
} 

void console_print_banner(void){
    (void)uart_protocol_send_text(CONSOLE_COLOR_CYAN CONSOLE_BOLD);
    (void)uart_protocol_send_text("db   dD d88888b .d8888. d888888b d8888b. d88888b db \r\n");
    (void)uart_protocol_send_text("88 ,8P' 88'     88'  YP `~~88~~' 88  `8D 88'     88\r\n");
    (void)uart_protocol_send_text("88,8P   88ooooo `8bo.      88    88oobY' 88ooooo 88\r\n");
    (void)uart_protocol_send_text("88`8b   88~~~~~   `Y8b.    88    88`8b   88~~~~~ 88\r\n");
    (void)uart_protocol_send_text("88 `88. 88.     db   8D    88    88 `88. 88.     88booo.\r\n");
    (void)uart_protocol_send_text("YP   YD Y88888P `8888Y'    YP    88   YD Y88888P Y88888P \r\n");
}

void console_print_menu(void){
    (void)uart_protocol_send_text(CONSOLE_BOLD "Boot Menu Options:\r\n" CONSOLE_COLOR_RESET);
    (void)uart_protocol_send_text(" [1] Boot App (autoboot)\r\n");
    (void)uart_protocol_send_text(" [2] Enter Update Mode\r\n\r\n");
}

void console_clear_screen(void){
    (void)uart_protocol_send_text(CONSOLE_CLEAR);
}
