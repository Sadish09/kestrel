#ifndef JUMP_H
#define JUMP_H

#include <stdint.h>

#include "boot_types.h"

boot_status_t jump_to_application(uint32_t app_base);

#endif
