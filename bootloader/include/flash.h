#ifndef FLASH_H
#define FLASH_H

#include <stddef.h>
#include <stdint.h>

#include "boot_types.h"

boot_status_t flash_unlock(void);
boot_status_t flash_lock(void);
boot_status_t flash_erase_page(uint32_t page_address);
boot_status_t flash_write(uint32_t address, const uint8_t *data, size_t length);
boot_status_t flash_read_metadata(boot_app_metadata_t *metadata);
boot_status_t flash_write_metadata(const boot_app_metadata_t *metadata);
boot_status_t flash_erase_app_region(uint32_t app_size);

#endif
