#include "../include/flash.h"

#include "../include/boot_config.h"

#define REG32(address) (*(volatile uint32_t *)(address))

#define FLASH_REG_BASE  0x40022000u
#define FLASH_KEYR  REG32(FLASH_REG_BASE + 0x04u)
#define FLASH_SR  REG32(FLASH_REG_BASE + 0x0Cu)
#define FLASH_CR  REG32(FLASH_REG_BASE + 0x10u)
#define FLASH_AR  REG32(FLASH_REG_BASE + 0x14u)

#define FLASH_KEY1  0x45670123u
#define FLASH_KEY2  0xCDEF89ABu

#define FLASH_SR_BSY  (1u << 0)
#define FLASH_SR_PGERR  (1u << 2)
#define FLASH_SR_WRPRTERR  (1u << 4)
#define FLASH_SR_EOP  (1u << 5)

#define FLASH_CR_PG  (1u << 0)
#define FLASH_CR_PER  (1u << 1)
#define FLASH_CR_MER  (1u << 2)
#define FLASH_CR_STRT  (1u << 6)
#define FLASH_CR_LOCK  (1u << 7)

static boot_status_t flash_wait_ready(void){
    volatile uint32_t timeout = 500000u;

    while ((FLASH_SR & FLASH_SR_BSY) != 0u){
        if (--timeout == 0u){
            return BOOT_ERR_FLASH;
        }
    }

    if ((FLASH_SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) != 0u){
        FLASH_SR = FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
        return BOOT_ERR_FLASH;
    }

    return BOOT_OK;
}

boot_status_t flash_unlock(void){
    if ((FLASH_CR & FLASH_CR_LOCK) != 0u){
        FLASH_KEYR = FLASH_KEY1;
        FLASH_KEYR = FLASH_KEY2;
    }

    if ((FLASH_CR & FLASH_CR_LOCK) != 0u){
        return BOOT_ERR_FLASH;
    }

    return BOOT_OK;
}

boot_status_t flash_lock(void){
    FLASH_CR |= FLASH_CR_LOCK;
    return BOOT_OK;
}

boot_status_t flash_erase_page(uint32_t page_address){
    boot_status_t status;

    // refuse addresses outside metadata page or app region
    if (page_address != BOOT_METADATA_ADDR &&
        (page_address < BOOT_APP_BASE || page_address >= (BOOT_APP_BASE + BOOT_APP_MAX_SIZE))){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    // address must be page-aligned (1 KiB)
    if ((page_address % BOOT_FLASH_PAGE_SIZE) != 0u){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    status = flash_wait_ready();
    if (status != BOOT_OK){
        return status;
    }

    FLASH_CR |= FLASH_CR_PER;
    FLASH_AR = page_address;
    FLASH_CR |= FLASH_CR_STRT;

    status = flash_wait_ready();

    FLASH_CR &= ~FLASH_CR_PER;

    return status;
}

boot_status_t flash_write(uint32_t address, const uint8_t *data, size_t length){
    boot_status_t status;
    size_t i;

    if (data == NULL || length == 0u){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    // refuse address ranges outside metadata or app region
    if ((address >= BOOT_METADATA_ADDR && (address + length) <= (BOOT_METADATA_ADDR + BOOT_FLASH_PAGE_SIZE)) ||
        (address >= BOOT_APP_BASE && (address + length) <= (BOOT_APP_BASE + BOOT_APP_MAX_SIZE))){
        // address range is valid
    } else {
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    // program half-words 
    for (i = 0u; i < length; i += 2u){
        uint16_t half_word;

        if (i + 1u < length){
            half_word = (uint16_t)data[i] | ((uint16_t)data[i + 1u] << 8);
        } else {
            half_word = (uint16_t)data[i] | 0xFF00u;
        }

        status = flash_wait_ready();
        if (status != BOOT_OK){
            return status;
        }

        FLASH_CR |= FLASH_CR_PG;
        *(volatile uint16_t *)(uintptr_t)(address + i) = half_word;
        status = flash_wait_ready();
        FLASH_CR &= ~FLASH_CR_PG;

        if (status != BOOT_OK){
            return status;
        }
    }

    // read back verification
    for (i = 0u; i < length; i++){
        if (*(const uint8_t *)(uintptr_t)(address + i) != data[i]){
            return BOOT_ERR_FLASH;
        }
    }

    return BOOT_OK;
}

boot_status_t flash_read_metadata(boot_app_metadata_t *metadata){
    const uint8_t *src;
    uint8_t *dst;
    size_t i;

    if (metadata == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    src = (const uint8_t *)BOOT_METADATA_ADDR;
    dst = (uint8_t *)metadata;

    for (i = 0u; i < sizeof(boot_app_metadata_t); i++){
        dst[i] = src[i];
    }

    return BOOT_OK;
}

boot_status_t flash_write_metadata(const boot_app_metadata_t *metadata){
    boot_status_t status;

    if (metadata == NULL){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    status = flash_unlock();
    if (status != BOOT_OK){
        return status;
    }

    status = flash_erase_page(BOOT_METADATA_ADDR);
    if (status != BOOT_OK){
        (void)flash_lock();
        return status;
    }

    status = flash_write(BOOT_METADATA_ADDR, (const uint8_t *)metadata, sizeof(boot_app_metadata_t));
    (void)flash_lock();

    return status;
}

boot_status_t flash_erase_app_region(uint32_t app_size){
    uint32_t num_pages;
    uint32_t page;
    boot_status_t status;

    if (app_size == 0u || app_size > BOOT_APP_MAX_SIZE){
        return BOOT_ERR_INVALID_ARGUMENT;
    }

    num_pages = (app_size + BOOT_FLASH_PAGE_SIZE - 1u) / BOOT_FLASH_PAGE_SIZE;

    status = flash_unlock();
    if (status != BOOT_OK){
        return status;
    }

    for (page = 0u; page < num_pages; page++){
        uint32_t page_addr = BOOT_APP_BASE + (page * BOOT_FLASH_PAGE_SIZE);
        status = flash_erase_page(page_addr);
        if (status != BOOT_OK){
            (void)flash_lock();
            return status;
        }
    }

    (void)flash_lock();
    return BOOT_OK;
}
