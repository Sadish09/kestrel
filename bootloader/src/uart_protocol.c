#include "../include/uart_protocol.h"
#include "../include/hardware.h"

#define REG32(address) (*(volatile uint32_t *)(address))

#define GPIOA_CRH REG32(0x40010804u)

#define USART1_SR REG32(0x40013800u)
#define USART1_DR REG32(0x40013804u)
#define USART1_BRR REG32(0x40013808u)
#define USART1_CR1 REG32(0x4001380Cu)

#define USART_SR_RXNE (1u << 5)
#define USART_SR_TXE (1u << 7)
#define USART_CR1_RE (1u << 2)
#define USART_CR1_TE (1u << 3)
#define USART_CR1_UE (1u << 13)

static void uart_protocol_send_char(char ch){
    while ((USART1_SR & USART_SR_TXE) == 0u)
    {
    }

    USART1_DR = (uint32_t)(uint8_t)ch;
}

boot_status_t uart_protocol_init(void)
{
    GPIOA_CRH &= ~((0xFu << 4) | (0xFu << 8));
    GPIOA_CRH |= (0xBu << 4); // PA9: USART1_TX alternate push-pull, 50 MHz
    GPIOA_CRH |= (0x4u << 8); // PA10: USART1_RX floating input

    USART1_CR1 = 0u;
    USART1_BRR = 0x0045u; // 115200 baud w default 8 MHz HSI clock
    USART1_CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    return BOOT_OK;
}

bool uart_protocol_char_available(void){
    return (USART1_SR & USART_SR_RXNE) != 0u;
}

bool uart_protocol_read_char(char *ch){
    if (!uart_protocol_char_available()){
        return false;
    }

    if (ch != 0){
        *ch = (char)(USART1_DR & 0xFFu);
    } else {
        (void)USART1_DR;
    }

    return true;
}

static bool uart_receive_byte_timeout(uint8_t *byte, uint32_t timeout_ms){
    /* ~1000 delays of 100 cycles approx 1 ms on 8 MHz HSI */
    uint32_t loops = timeout_ms * 80u;

    while (loops > 0u){
        if (uart_protocol_read_char((char *)byte)){
            return true;
        }
        hardware_delay(100u);
        loops--;
    }

    return false;
}

boot_status_t uart_protocol_receive_packet(uart_packet_t *packet, uint32_t timeout_ms){
    uint8_t rx_byte;
    uint8_t header[8];
    uint8_t calc_checksum = 0u;
    uint16_t length;
    uint32_t offset;
    uint16_t i;

    if (packet == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    // wait for sync byte (0xAA)
    while (1){
        if (!uart_receive_byte_timeout(&rx_byte, timeout_ms)){
            return BOOT_ERR_UART;
        }
        if (rx_byte == UART_SYNC_BYTE){
            break;
        }
    }

    // read header bytes: type(1), offset(4 LE), length(2 LE), checksum(1)
    for (i = 0u; i < 8u; i++){
        if (!uart_receive_byte_timeout(&header[i], 100u)){
            return BOOT_ERR_UART;
        }
    }

    packet->type = (uart_packet_type_t)header[0];
    offset = (uint32_t)header[1] | ((uint32_t)header[2] << 8) | ((uint32_t)header[3] << 16) | ((uint32_t)header[4] << 24);
    length = (uint16_t)header[5] | ((uint16_t)header[6] << 8);

    if (length > UART_MAX_PAYLOAD){
        return BOOT_ERR_UART;
    }

    packet->offset = offset;
    packet->length = length;

    // receive payload bytes
    for (i = 0u; i < length; i++){
        if (!uart_receive_byte_timeout(&packet->payload[i], 100u)){
            return BOOT_ERR_UART;
        }
    }

    // verify checksum (XOR sum of type, offset, length, payload)
    calc_checksum ^= header[0];
    for (i = 1u; i <= 6u; i++){
        calc_checksum ^= header[i];
    }
    for (i = 0u; i < length; i++){
        calc_checksum ^= packet->payload[i];
    }

    if (calc_checksum != header[7]){
        return BOOT_ERR_UART;
    }

    return BOOT_OK;
}

boot_status_t uart_protocol_send_status(boot_status_t status){
    if (status == BOOT_OK){
        return uart_protocol_send_text("OK\r\n");
    }

    return uart_protocol_send_text("ERR\r\n");
}

boot_status_t uart_protocol_send_text(const char *text){
    if (text == 0){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    while (*text != '\0'){
        uart_protocol_send_char(*text);
        text++;
    }

    return BOOT_OK;
}
