#include "../include/crypto.h"

#include "../include/boot_config.h"
#include "../include/boot_public_key.h"

#define LONESHA256_IMPLEMENTATION
#include "../../third_party/lonesha256/lonesha256.h"

#define uECC_SUPPORTS_secp160r1 0
#define uECC_SUPPORTS_secp192r1 0
#define uECC_SUPPORTS_secp256r1 1
#define uECC_SUPPORTS_secp256k1 0
#define uECC_OPTIMIZATION_LEVEL 2
#include "../../third_party/micro-ecc/uECC.h"

boot_status_t crypto_init(void){
    return BOOT_OK;
}

boot_status_t crypto_sha256(const uint8_t *data, size_t length, uint8_t hash[BOOT_HASH_SIZE]){
    if (data == NULL || hash == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    if (lonesha256((unsigned char *)hash, (const unsigned char *)data, length) != 0){
        return BOOT_ERR_CRYPTO;
    }

    return BOOT_OK;
}

boot_status_t crypto_sha256_start(boot_sha256_context_t *context){
    (void)context;
    return BOOT_OK;
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
    int res;

    if (hash == NULL || signature == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    res = uECC_verify(KESTREL_PUBLIC_KEY, hash, (unsigned)BOOT_HASH_SIZE, signature, uECC_secp256r1());

    if (res == 1){
        return BOOT_OK;
    }

    return BOOT_ERR_SIGNATURE_FAILED;
}
