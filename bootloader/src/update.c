#include "../include/update.h"

boot_status_t update_run(void){
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t update_begin(const boot_update_request_t *request){
    (void)request;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t update_write_chunk(uint32_t offset, const uint8_t *data, size_t length){
    (void)offset;
    (void)data;
    (void)length;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t update_finish(void){
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t update_abort(void){
    return BOOT_ERR_NOT_IMPLEMENTED;
}
