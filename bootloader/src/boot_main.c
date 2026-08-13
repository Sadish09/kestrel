#include "../include/boot_main.h"

#include "../include/app_image.h"
#include "../include/boot_config.h"
#include "../include/hardware.h"
#include "../include/jump.h"
#include "../include/uart_protocol.h"

static void bootloader_blink_loop(void){
    while (1){
        hardware_led_toggle();
        hardware_delay(200000u);
    }
}

boot_status_t boot_main(void){
    boot_status_t status;
    boot_app_metadata_t metadata;

    status = hardware_init();

    if (status != BOOT_OK){
        return status;
    }

    status = uart_protocol_init();

    if (status != BOOT_OK){
        return status;
    }

    (void)uart_protocol_send_text("Kestrel bootloader stage 5\r\n");

    if (boot_select_mode() == BOOT_MODE_UPDATE){
        (void)uart_protocol_send_text("Mode: bootloader requested\r\n");
        bootloader_blink_loop();
    }

    (void)uart_protocol_send_text("Mode: try application\r\n");

    /* Read metadata from 0x08003C00 */
    status = app_image_read_metadata(&metadata);

    if (status == BOOT_OK){
        /* Perform full cryptographic validation (SHA-256 hash + ECDSA signature) */
        boot_status_t val_status = app_image_validate(&metadata);

        if (val_status == BOOT_OK){
            (void)uart_protocol_send_text("App hash & signature OK, jumping\r\n");
            return jump_to_application(metadata.app_base);
        } else if (val_status == BOOT_ERR_INVALID_IMAGE){
            (void)uart_protocol_send_text("Validation failed: SHA-256 hash mismatch!\r\n");
        } else if (val_status == BOOT_ERR_SIGNATURE_FAILED){
            (void)uart_protocol_send_text("Validation failed: ECDSA signature failed!\r\n");
        } else {
            (void)uart_protocol_send_text("Validation failed: invalid metadata struct\r\n");
        }
    } else {
        (void)uart_protocol_send_text("Metadata read failed\r\n");
    }

#ifdef BOOT_DEV_MODE
    /* Dev escape hatch: if validation fails but BOOT_DEV_MODE is set,
     * check vector table sanity and jump anyway. */
    (void)uart_protocol_send_text("DEV_MODE: checking vector table only\r\n");

    if (app_image_vector_table_is_sane(BOOT_APP_BASE)){
        (void)uart_protocol_send_text("DEV_MODE: jumping to app\r\n");
        return jump_to_application(BOOT_APP_BASE);
    }

    (void)uart_protocol_send_text("DEV_MODE: no valid vector table either\r\n");
#endif

    (void)uart_protocol_send_text("Staying in bootloader mode\r\n");
    bootloader_blink_loop();

    return BOOT_ERR_INVALID_STATE;
}

boot_mode_t boot_select_mode(void){
    if (boot_update_requested()){
        return BOOT_MODE_UPDATE;
    }

    return BOOT_MODE_TRY_APPLICATION;
}

bool boot_update_requested(void){
    return hardware_boot_pin_active();
}
