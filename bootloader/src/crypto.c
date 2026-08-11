#include "../include/crypto.h"

boot_status_t crypto_init(void){
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t crypto_sha256_start(boot_sha256_context_t *context){
    (void)context;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t crypto_sha256_update(boot_sha256_context_t *context, const uint8_t *data, size_t length){
    (void)context;
    (void)data;
    (void)length;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t crypto_sha256_finish(boot_sha256_context_t *context, uint8_t hash[BOOT_HASH_SIZE]){
    (void)context;
    (void)hash;
    return BOOT_ERR_NOT_IMPLEMENTED;
}

boot_status_t crypto_verify_signature(const uint8_t hash[BOOT_HASH_SIZE], const uint8_t signature[BOOT_SIGNATURE_SIZE]){
    (void)hash;
    (void)signature;
    return BOOT_ERR_NOT_IMPLEMENTED;
}
