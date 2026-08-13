#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

#include <stdint.h>

#define BOOT_FLASH_BASE ((uint32_t)0x08000000u)
#define BOOT_FLASH_SIZE ((uint32_t)(64u * 1024u))
#define BOOT_FLASH_PAGE_SIZE ((uint32_t)1024u)

#define BOOTLOADER_BASE BOOT_FLASH_BASE
#define BOOTLOADER_SIZE ((uint32_t)(16u * 1024u))

#define BOOT_METADATA_ADDR ((uint32_t)0x08003C00u)
#define BOOT_APP_BASE ((uint32_t)0x08004000u)
#define BOOT_APP_MAX_SIZE ((uint32_t)(BOOT_FLASH_SIZE - BOOTLOADER_SIZE))

#define BOOT_SRAM_BASE ((uint32_t)0x20000000u)
#define BOOT_SRAM_SIZE ((uint32_t)(20u * 1024u))
#define BOOT_SRAM_END ((uint32_t)(BOOT_SRAM_BASE + BOOT_SRAM_SIZE))

#define BOOT_METADATA_MAGIC ((uint32_t)0x4B455354u)
#define BOOT_METADATA_VERSION ((uint32_t)1u)

#define BOOT_HASH_SIZE ((uint32_t)32u)
#define BOOT_SIGNATURE_SIZE ((uint32_t)64u)
#define BOOT_PUBLIC_KEY_SIZE ((uint32_t)64u)

/* Development mode disabled for secure boot enforcement test */
/* #define BOOT_DEV_MODE 1 */

#endif

// Full specification will be updated in spec.md, if ever changed
