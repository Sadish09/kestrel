#ifndef APP_IMAGE_H
#define APP_IMAGE_H

#include <stdbool.h>
#include <stdint.h>

#include "boot_types.h"

boot_status_t app_image_read_metadata(boot_app_metadata_t *metadata);
bool app_image_metadata_is_sane(const boot_app_metadata_t *metadata);
bool app_image_vector_table_is_sane(uint32_t app_base);
boot_status_t app_image_validate(const boot_app_metadata_t *metadata);

#endif
