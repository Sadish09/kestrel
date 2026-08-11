#ifndef BOOT_TYPES_H
#define BOOT_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "boot_config.h"

typedef enum{
    BOOT_OK = 0,
    BOOT_ERR_NOT_IMPLEMENTED,
    BOOT_ERR_INVALID_ARGUMENT,
    BOOT_ERR_INVALID_STATE,
    BOOT_ERR_FLASH,
    BOOT_ERR_UART,
    BOOT_ERR_CRYPTO,
    BOOT_ERR_INVALID_IMAGE,
    BOOT_ERR_INVALID_METADATA,
    BOOT_ERR_SIGNATURE_FAILED
} boot_status_t;

typedef struct{
    uint32_t magic;
    uint32_t metadata_version;
    uint32_t app_version;
    uint32_t app_base;
    uint32_t app_size;
    uint8_t app_hash[BOOT_HASH_SIZE];
    uint8_t app_signature[BOOT_SIGNATURE_SIZE];
} boot_app_metadata_t;

typedef struct{
    uint32_t firmware_size;
    uint32_t firmware_version;
    uint8_t signature[BOOT_SIGNATURE_SIZE];
} boot_update_request_t;

typedef enum{
    BOOT_MODE_TRY_APPLICATION = 0,
    BOOT_MODE_UPDATE
} boot_mode_t;

#endif
