#include "../include/jump.h"

#define REG32(address) (*(volatile uint32_t *)(address))

#define SCB_VTOR REG32(0xE000ED08u)
#define SYST_CSR REG32(0xE000E010u)

#define NVIC_ICER0 REG32(0xE000E180u)
#define NVIC_ICER1 REG32(0xE000E184u)
#define NVIC_ICPR0 REG32(0xE000E280u)
#define NVIC_ICPR1 REG32(0xE000E284u)

typedef void (*app_reset_handler_t)(void);

boot_status_t jump_to_application(uint32_t app_base){
    uint32_t app_stack = *((const uint32_t *)(uintptr_t)app_base);
    uint32_t app_reset = *((const uint32_t *)(uintptr_t)(app_base + 4u));
    app_reset_handler_t reset_handler = (app_reset_handler_t)(uintptr_t)app_reset;

    __asm volatile ("cpsid i");

    SYST_CSR = 0u;

    NVIC_ICER0 = 0xFFFFFFFFu;
    NVIC_ICER1 = 0xFFFFFFFFu;
    NVIC_ICPR0 = 0xFFFFFFFFu;
    NVIC_ICPR1 = 0xFFFFFFFFu;

    SCB_VTOR = app_base;

    __asm volatile ("msr msp, %0" : : "r" (app_stack) : );
    __asm volatile ("cpsie i");

    reset_handler();

    return BOOT_ERR_INVALID_STATE;
}
