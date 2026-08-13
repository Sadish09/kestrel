#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#include "boot_types.h"

typedef struct{
    uint32_t reserved;
} boot_sha256_context_t;

boot_status_t crypto_init(void);
boot_status_t crypto_sha256(const uint8_t *data, size_t length, uint8_t hash[BOOT_HASH_SIZE]);
boot_status_t crypto_sha256_start(boot_sha256_context_t *context);
boot_status_t crypto_sha256_update(boot_sha256_context_t *context, const uint8_t *data, size_t length);
boot_status_t crypto_sha256_finish(boot_sha256_context_t *context, uint8_t hash[BOOT_HASH_SIZE]);
boot_status_t crypto_verify_signature(const uint8_t hash[BOOT_HASH_SIZE], const uint8_t signature[BOOT_SIGNATURE_SIZE]);

#endif
