#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>
#include <stdint.h>

#include "boot_types.h"

boot_status_t hardware_init(void);
void hardware_led_on(void);
void hardware_led_off(void);
void hardware_led_toggle(void);
void hardware_delay(volatile uint32_t cycles);
bool hardware_boot_pin_active(void);

#endif
