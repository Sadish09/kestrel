#include "../include/boot_main.h"

#include "../include/app_image.h"
#include "../include/boot_config.h"
#include "../include/console.h"
#include "../include/hardware.h"
#include "../include/jump.h"
#include "../include/uart_protocol.h"
#include "../include/update.h"

boot_status_t boot_main(void){
    boot_status_t status;
    boot_app_metadata_t metadata;
    char key;

    status = hardware_init();

    if (status != BOOT_OK){
        return status;
    }

    console_init();
    console_print_banner();

    if (boot_select_mode() == BOOT_MODE_UPDATE){
        (void)uart_protocol_send_text(CONSOLE_COLOR_YELLOW "Mode: Update forced via PB12 pin\r\n" CONSOLE_COLOR_RESET);
        return update_run();
    }

    /* Read metadata from 0x08003C00 */
    status = app_image_read_metadata(&metadata);

    if (status == BOOT_OK){
        /* Perform full cryptographic validation (SHA-256 hash + ECDSA signature) */
        boot_status_t val_status = app_image_validate(&metadata);

        if (val_status == BOOT_OK){
            uint32_t countdown = 3u;
            bool enter_update = false;

            (void)uart_protocol_send_text("App Security: " CONSOLE_COLOR_GREEN CONSOLE_BOLD "VALIDATED (SHA-256 + ECDSA)\r\n" CONSOLE_COLOR_RESET);
            console_print_menu();

            /* Interactive countdown loop */
            while (countdown > 0u && !enter_update){
                (void)uart_protocol_send_text("Booting in ");
                char count_str[8];
                count_str[0] = (char)('0' + countdown);
                count_str[1] = 's';
                count_str[2] = '.';
                count_str[3] = '.';
                count_str[4] = ' ';
                count_str[5] = '\r';
                count_str[6] = '\0';
                (void)uart_protocol_send_text(count_str);

                /* Poll for keypress for ~1 second */
                for (uint32_t i = 0u; i < 100u; i++){
                    if (uart_protocol_read_char(&key)){
                        if (key == '2'){
                            enter_update = true;
                            break;
                        } else if (key == '1'){
                            countdown = 0u;
                            break;
                        }
                    }
                    hardware_delay(10000u);
                }

                if (countdown > 0u){
                    countdown--;
                }
            }

            (void)uart_protocol_send_text("\r\n");

            if (enter_update){
                (void)uart_protocol_send_text(CONSOLE_COLOR_YELLOW "Update Mode requested via UART\r\n" CONSOLE_COLOR_RESET);
                return update_run();
            }

            /* Clean terminal handoff: clear screen before calling app */
            console_clear_screen();
            return jump_to_application(metadata.app_base);
        } else if (val_status == BOOT_ERR_INVALID_IMAGE){
            (void)uart_protocol_send_text(CONSOLE_COLOR_RED "Validation Failed: SHA-256 Hash Mismatch!\r\n" CONSOLE_COLOR_RESET);
        } else if (val_status == BOOT_ERR_SIGNATURE_FAILED){
            (void)uart_protocol_send_text(CONSOLE_COLOR_RED "Validation Failed: Signature Verification Failed!\r\n" CONSOLE_COLOR_RESET);
        } else {
            (void)uart_protocol_send_text(CONSOLE_COLOR_RED "Validation Failed: Malformed Metadata\r\n" CONSOLE_COLOR_RESET);
        }
    } else {
        (void)uart_protocol_send_text(CONSOLE_COLOR_YELLOW "Metadata page uninitialized\r\n" CONSOLE_COLOR_RESET);
    }

#ifdef BOOT_DEV_MODE
    /* Dev escape hatch */
    (void)uart_protocol_send_text("DEV_MODE: checking vector table only...\r\n");

    if (app_image_vector_table_is_sane(BOOT_APP_BASE)){
        (void)uart_protocol_send_text("DEV_MODE: jumping to app\r\n");
        console_clear_screen();
        return jump_to_application(BOOT_APP_BASE);
    }

    (void)uart_protocol_send_text("DEV_MODE: no valid vector table found\r\n");
#endif

    (void)uart_protocol_send_text(CONSOLE_COLOR_RED "Entering Bootloader Update Mode\r\n" CONSOLE_COLOR_RESET);
    return update_run();
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
