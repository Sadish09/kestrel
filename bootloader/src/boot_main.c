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

    (void)uart_protocol_send_text("Kestrel bootloader stage 4\r\n");

    if (boot_select_mode() == BOOT_MODE_UPDATE){
        (void)uart_protocol_send_text("Mode: bootloader requested\r\n");
        bootloader_blink_loop();
    }

    (void)uart_protocol_send_text("Mode: try application\r\n");

    /* Stage 4: read and validate metadata before jumping. */
    status = app_image_read_metadata(&metadata);

    if (status == BOOT_OK && app_image_metadata_is_sane(&metadata)){
        (void)uart_protocol_send_text("Metadata OK\r\n");

        if (app_image_vector_table_is_sane(metadata.app_base)){
            (void)uart_protocol_send_text("Vector table OK, jumping\r\n");
            return jump_to_application(metadata.app_base);
        }

        (void)uart_protocol_send_text("Vector table invalid\r\n");
    } else {
        (void)uart_protocol_send_text("No valid metadata\r\n");
    }

#ifdef BOOT_DEV_MODE
    /* Dev escape hatch: if metadata is missing or invalid but there is
     * a plausible vector table at BOOT_APP_BASE, jump anyway.
     * This allows flashing a raw app without metadata during development.
     * Remove BOOT_DEV_MODE once metadata/crypto is fully wired. */
    (void)uart_protocol_send_text("DEV_MODE: checking vector table only\r\n");

    if (app_image_vector_table_is_sane(BOOT_APP_BASE)){
        (void)uart_protocol_send_text("DEV_MODE: jumping to app\r\n");
        return jump_to_application(BOOT_APP_BASE);
    }

    (void)uart_protocol_send_text("DEV_MODE: no valid vector table either\r\n");
#endif

    (void)uart_protocol_send_text("Staying in bootloader\r\n");
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
