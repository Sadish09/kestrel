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
    boot_status_t status = hardware_init();

    if (status != BOOT_OK){
        return status;
    }

    status = uart_protocol_init();

    if (status != BOOT_OK){
        return status;
    }

    (void)uart_protocol_send_text("Kestrel bootloader stage 3\r\n");

    if (boot_select_mode() == BOOT_MODE_UPDATE){
        (void)uart_protocol_send_text("Mode: bootloader requested\r\n");
        bootloader_blink_loop();
    }

    (void)uart_protocol_send_text("Mode: try application\r\n");

    if (app_image_vector_table_is_sane(BOOT_APP_BASE)){
        (void)uart_protocol_send_text("App vector table OK, jumping\r\n");
        return jump_to_application(BOOT_APP_BASE);
    }

    (void)uart_protocol_send_text("No valid app vector table, staying in bootloader\r\n");
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
