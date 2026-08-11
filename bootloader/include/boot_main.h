#ifndef BOOT_MAIN_H
#define BOOT_MAIN_H

#include "boot_types.h"

boot_status_t boot_main(void);
boot_mode_t boot_select_mode(void);
bool boot_update_requested(void);

#endif
