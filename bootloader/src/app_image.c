#include "../include/app_image.h"

#include "../include/boot_config.h"
#include "../include/flash.h"

boot_status_t app_image_read_metadata(boot_app_metadata_t *metadata){
    if (metadata == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    return flash_read_metadata(metadata);
}

bool app_image_metadata_is_sane(const boot_app_metadata_t *metadata){
    if (metadata == NULL){
        return false;
    }

    if (metadata->magic != BOOT_METADATA_MAGIC){
        return false;
    }

    if (metadata->metadata_version != BOOT_METADATA_VERSION){
        return false;
    }

    if (metadata->app_base != BOOT_APP_BASE){
        return false;
    }

    if (metadata->app_size == 0u){
        return false;
    }

    if (metadata->app_size > BOOT_APP_MAX_SIZE){
        return false;
    }

    return true;
}

bool app_image_vector_table_is_sane(uint32_t app_base){
    uint32_t initial_sp;
    uint32_t reset_handler;
    uint32_t reset_address;

    if (app_base != BOOT_APP_BASE){
        return false;
    }

    initial_sp = *((const uint32_t *)(uintptr_t)app_base);
    reset_handler = *((const uint32_t *)(uintptr_t)(app_base + 4u));
    reset_address = reset_handler & ~1u;

    if (initial_sp < BOOT_SRAM_BASE || initial_sp > BOOT_SRAM_END){
        return false;
    }

    if ((reset_handler & 1u) == 0u){
        return false;
    }

    if (reset_address < BOOT_APP_BASE){
        return false;
    }

    if (reset_address >= (BOOT_APP_BASE + BOOT_APP_MAX_SIZE)){
        return false;
    }

    return true;
}

boot_status_t app_image_validate(const boot_app_metadata_t *metadata){
    /* Stage 4: structural checks only. */
    /* Stage 5 will add hash and signature verification here. */

    if (!app_image_metadata_is_sane(metadata)){
        return BOOT_ERR_INVALID_METADATA;
    }

    if (!app_image_vector_table_is_sane(metadata->app_base)){
        return BOOT_ERR_INVALID_IMAGE;
    }

    /* Crypto verification not implemented yet. */
    /* When Stage 5 is done, this will call crypto_sha256 and */
    /* crypto_verify_signature before returning BOOT_OK. */
    return BOOT_ERR_NOT_IMPLEMENTED;
}
