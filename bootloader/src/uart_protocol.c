#include "../include/uart_protocol.h"

#define REG32(address) (*(volatile uint32_t *)(address))

#define GPIOA_CRH REG32(0x40010804u)

#define USART1_SR REG32(0x40013800u)
#define USART1_DR REG32(0x40013804u)
#define USART1_BRR REG32(0x40013808u)
#define USART1_CR1 REG32(0x4001380Cu)

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

boot_status_t uart_protocol_receive_packet(uart_packet_t *packet){
    (void)packet;
    return BOOT_ERR_NOT_IMPLEMENTED;
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
