#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "boot_types.h"

typedef enum{
    UART_PACKET_HELLO = 1,
    UART_PACKET_BEGIN,
    UART_PACKET_DATA,
    UART_PACKET_END,
    UART_PACKET_ABORT
} uart_packet_type_t;

typedef struct{
    uart_packet_type_t type;
    uint32_t offset;
    uint16_t length;
    uint8_t payload[256];
} uart_packet_t;

boot_status_t uart_protocol_init(void);
boot_status_t uart_protocol_receive_packet(uart_packet_t *packet);
boot_status_t uart_protocol_send_status(boot_status_t status);
boot_status_t uart_protocol_send_text(const char *text);

bool uart_protocol_char_available(void);
bool uart_protocol_read_char(char *ch);

#endif
