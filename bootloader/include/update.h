#ifndef UPDATE_H
#define UPDATE_H

#include <stddef.h>
#include <stdint.h>

#include "boot_types.h"

boot_status_t update_run(void);
boot_status_t update_begin(const boot_update_request_t *request);
boot_status_t update_write_chunk(uint32_t offset, const uint8_t *data, size_t length);
boot_status_t update_finish(void);
boot_status_t update_abort(void);

#endif
