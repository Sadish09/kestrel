#include "../include/flash.h"

boot_status_t flash_unlock(void){
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t flash_lock(void){
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t flash_erase_page(uint32_t page_address){
    (void)page_address;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t flash_write(uint32_t address, const uint8_t *data, size_t length){
    (void)address;
    (void)data;
    (void)length;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t flash_read_metadata(boot_app_metadata_t *metadata){
    (void)metadata;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t flash_write_metadata(const boot_app_metadata_t *metadata){
    (void)metadata;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t flash_erase_app_region(uint32_t app_size){
    (void)app_size;
    return BOOT_ERR_NOT_IMPLEMENTED;
}
