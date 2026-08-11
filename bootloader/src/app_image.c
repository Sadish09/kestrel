#include "../include/app_image.h"

#include "../include/boot_config.h"

boot_status_t app_image_read_metadata(boot_app_metadata_t *metadata){
    (void)metadata;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

bool app_image_metadata_is_sane(const boot_app_metadata_t *metadata){
    (void)metadata;
    return false;
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
    (void)metadata;
    return BOOT_ERR_NOT_IMPLEMENTED;
}
