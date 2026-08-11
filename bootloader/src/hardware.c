#include "../include/hardware.h"

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_APB2ENR REG32(0x40021018u)

#define GPIOA_CRL REG32(0x40010800u)
#define GPIOA_CRH REG32(0x40010804u)
#define GPIOA_ODR REG32(0x4001080Cu)

#define GPIOB_CRH REG32(0x40010C04u)
#define GPIOB_IDR REG32(0x40010C08u)
#define GPIOB_ODR REG32(0x40010C0Cu)

#define GPIOC_CRH REG32(0x40011004u)
#define GPIOC_ODR REG32(0x4001100Cu)

#define RCC_APB2ENR_AFIOEN  (1u << 0)
#define RCC_APB2ENR_IOPAEN  (1u << 2)
#define RCC_APB2ENR_IOPBEN  (1u << 3)
#define RCC_APB2ENR_IOPCEN  (1u << 4)
#define RCC_APB2ENR_USART1EN (1u << 14)

#define LED_PIN 13u
#define BOOT_PIN 12u

boot_status_t hardware_init(void){
    RCC_APB2ENR |= RCC_APB2ENR_AFIOEN |
                   RCC_APB2ENR_IOPAEN |
                   RCC_APB2ENR_IOPBEN |
                   RCC_APB2ENR_IOPCEN |
                   RCC_APB2ENR_USART1EN;

    GPIOC_CRH &= ~(0xFu << 20);
    GPIOC_CRH |= (0x2u << 20); // PC13: output push-pull, 2 MHz
    hardware_led_off();

    GPIOB_CRH &= ~(0xFu << 16);
    GPIOB_CRH |= (0x8u << 16); // PB12: input with pull-up/down
    GPIOB_ODR |= (1u << BOOT_PIN); // Pull-up: grounded PB12 requests update mode

    (void)GPIOA_CRL;
    (void)GPIOA_CRH;
    (void)GPIOA_ODR;

    return BOOT_OK;
}

void hardware_led_on(void){
    GPIOC_ODR &= ~(1u << LED_PIN);
}

void hardware_led_off(void){
    GPIOC_ODR |= (1u << LED_PIN);
}

void hardware_led_toggle(void){
    GPIOC_ODR ^= (1u << LED_PIN);
}

void hardware_delay(volatile uint32_t cycles){
    while (cycles > 0u){
        cycles--;
    }
}

bool hardware_boot_pin_active(void){
    return (GPIOB_IDR & (1u << BOOT_PIN)) == 0u;
}
