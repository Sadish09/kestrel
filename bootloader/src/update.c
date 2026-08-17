#include "../include/update.h"

#include "../include/boot_config.h"
#include "../include/crypto.h"
#include "../include/flash.h"
#include "../include/hardware.h"
#include "../include/uart_protocol.h"

#define REG32(address) (*(volatile uint32_t *)(address))
#define SCB_AIRCR REG32(0xE000ED0Cu)

typedef enum{
    UPDATE_STATE_IDLE = 0,
    UPDATE_STATE_RECEIVING,
    UPDATE_STATE_VERIFYING,
    UPDATE_STATE_ERROR
} update_state_t;

static update_state_t g_update_state = UPDATE_STATE_IDLE;
static boot_update_request_t g_update_request;
static uint32_t g_received_bytes = 0u;

static void nvic_system_reset(void){
    __asm__ volatile ("cpsid i");
    SCB_AIRCR = 0x05FA0004u;
    while (1){
    }
}

boot_status_t update_begin(const boot_update_request_t *request){
    boot_status_t status;

    if (request == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    if (request->firmware_size == 0u || request->firmware_size > BOOT_APP_MAX_SIZE){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    g_update_request = *request;
    g_received_bytes = 0u;

    // erase application flash region
    status = flash_erase_app_region(request->firmware_size);
    if (status != BOOT_OK){
        g_update_state = UPDATE_STATE_ERROR;
        return status;
    }

    // erase metadata page for atomicity guarantee
    status = flash_unlock();
    if (status == BOOT_OK){
        (void)flash_erase_page(BOOT_METADATA_ADDR);
        (void)flash_lock();
    }

    g_update_state = UPDATE_STATE_RECEIVING;
    return BOOT_OK;
}

boot_status_t update_write_chunk(uint32_t offset, const uint8_t *data, size_t length){
    boot_status_t status;

    if (g_update_state != UPDATE_STATE_RECEIVING){
        return BOOT_ERR_INVALID_STATE;
    }

    if (data == NULL || length == 0u){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    // reject out-of-order writes for v1 sequential updates
    if (offset != g_received_bytes){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    if ((offset + length) > g_update_request.firmware_size){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    status = flash_unlock();
    if (status != BOOT_OK){
        return status;
    }

    status = flash_write(BOOT_APP_BASE + offset, data, length);
    (void)flash_lock();

    if (status != BOOT_OK){
        g_update_state = UPDATE_STATE_ERROR;
        return status;
    }

    g_received_bytes += (uint32_t)length;
    return BOOT_OK;
}

boot_status_t update_finish(void){
    boot_status_t status;
    boot_app_metadata_t metadata;
    uint8_t calculated_hash[BOOT_HASH_SIZE];
    const uint8_t *app_flash_ptr;
    uint32_t i;

    if (g_update_state != UPDATE_STATE_RECEIVING){
        return BOOT_ERR_INVALID_STATE;
    }

    if (g_received_bytes != g_update_request.firmware_size){
        return BOOT_ERR_INVALID_STATE;
    }

    g_update_state = UPDATE_STATE_VERIFYING;

    // compute SHA-256 over programmed application flash
    app_flash_ptr = (const uint8_t *)(uintptr_t)BOOT_APP_BASE;
    status = crypto_sha256(app_flash_ptr, (size_t)g_update_request.firmware_size, calculated_hash);
    if (status != BOOT_OK){
        g_update_state = UPDATE_STATE_ERROR;
        return status;
    }

    // verify ECDSA P-256 signature using embedded public key
    status = crypto_verify_signature(calculated_hash, g_update_request.signature);
    if (status != BOOT_OK){
        g_update_state = UPDATE_STATE_ERROR;
        return BOOT_ERR_SIGNATURE_FAILED;
    }

    // atomic commit
    metadata.magic = BOOT_METADATA_MAGIC;
    metadata.metadata_version = BOOT_METADATA_VERSION;
    metadata.app_version = g_update_request.firmware_version;
    metadata.app_base = BOOT_APP_BASE;
    metadata.app_size = g_update_request.firmware_size;

    for (i = 0u; i < BOOT_HASH_SIZE; i++){
        metadata.app_hash[i] = calculated_hash[i];
    }
    for (i = 0u; i < BOOT_SIGNATURE_SIZE; i++){
        metadata.app_signature[i] = g_update_request.signature[i];
    }

    status = flash_write_metadata(&metadata);
    if (status != BOOT_OK){
        g_update_state = UPDATE_STATE_ERROR;
        return status;
    }

    // update complete; send OK ACK, wait for UART TX to clear, and reboot device
    (void)uart_protocol_send_status(BOOT_OK);
    hardware_delay(200000u);
    nvic_system_reset();

    return BOOT_OK;
}

boot_status_t update_abort(void){
    g_update_state = UPDATE_STATE_IDLE;
    g_received_bytes = 0u;
    return BOOT_OK;
}

boot_status_t update_run(void){
    uart_packet_t packet;
    boot_status_t status;

    (void)uart_protocol_send_text("Update Mode Active. Listening for packets...\r\n");

    while (1){
        hardware_led_toggle();

        // receive binary packet with 10s timeout
        status = uart_protocol_receive_packet(&packet, 10000u);

        if (status != BOOT_OK){
            // timeout or frame error; if we were mid-receive, send ERR and keep LED blinking
            continue;
        }

        switch (packet.type){
        case UART_PACKET_HELLO:
            (void)uart_protocol_send_status(BOOT_OK);
            break;

        case UART_PACKET_BEGIN:
            if (packet.length >= (4u + 4u + BOOT_SIGNATURE_SIZE)){
                boot_update_request_t req;
                req.firmware_size = (uint32_t)packet.payload[0] | ((uint32_t)packet.payload[1] << 8) | ((uint32_t)packet.payload[2] << 16) | ((uint32_t)packet.payload[3] << 24);
                req.firmware_version = (uint32_t)packet.payload[4] | ((uint32_t)packet.payload[5] << 8) | ((uint32_t)packet.payload[6] << 16) | ((uint32_t)packet.payload[7] << 24);

                for (uint32_t k = 0u; k < BOOT_SIGNATURE_SIZE; k++){
                    req.signature[k] = packet.payload[8u + k];
                }

                status = update_begin(&req);
                (void)uart_protocol_send_status(status);
            } else {
                (void)uart_protocol_send_status(BOOT_ERR_INVALID_ARGUMENT);
            }
            break;

        case UART_PACKET_DATA:
            status = update_write_chunk(packet.offset, packet.payload, (size_t)packet.length);
            (void)uart_protocol_send_status(status);
            break;

        case UART_PACKET_END:
            status = update_finish();
            /* If update_finish succeeded, system reset happened inside update_finish.
             * If signature failed or error occurred, send status here. */
            (void)uart_protocol_send_status(status);
            break;

        case UART_PACKET_ABORT:
            (void)update_abort();
            (void)uart_protocol_send_status(BOOT_OK);
            break;

        default:
            (void)uart_protocol_send_status(BOOT_ERR_INVALID_ARGUMENT);
            break;
        }
    }
}
