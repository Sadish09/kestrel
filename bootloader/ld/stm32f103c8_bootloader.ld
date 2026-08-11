/* STM32F103C8 Blue Pill bootloader linker script.
 *
 * Conservative 64 KiB layout:
 * - 0x08000000..0x08003BFF: bootloader code/data in flash
 * - 0x08003C00..0x08003FFF: boot metadata page
 * - 0x08004000..0x0800FFFF: application image
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH_BOOT (rx)  : ORIGIN = 0x08000000, LENGTH = 15K
    FLASH_META (rx)  : ORIGIN = 0x08003C00, LENGTH = 1K
    FLASH_APP  (rx)  : ORIGIN = 0x08004000, LENGTH = 48K
    RAM        (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
_bootloader_start = ORIGIN(FLASH_BOOT);
_bootloader_end = ORIGIN(FLASH_BOOT) + LENGTH(FLASH_BOOT);
_boot_metadata_start = ORIGIN(FLASH_META);
_boot_metadata_end = ORIGIN(FLASH_META) + LENGTH(FLASH_META);
_app_start = ORIGIN(FLASH_APP);
_app_end = ORIGIN(FLASH_APP) + LENGTH(FLASH_APP);

PHDRS
{
    boot PT_LOAD FLAGS(5); /* read + execute */
    ram PT_LOAD FLAGS(6);  /* read + write */
    meta PT_LOAD FLAGS(4); /* read only */
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH_BOOT :boot

    .text :
    {
        *(.text*)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
    } > FLASH_BOOT :boot

    .ARM.extab :
    {
        *(.ARM.extab*)
    } > FLASH_BOOT :boot

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH_BOOT :boot

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH_BOOT :ram

    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM :ram

    .boot_metadata (NOLOAD) :
    {
        . = ALIGN(4);
        KEEP(*(.boot_metadata))
        . = ALIGN(4);
    } > FLASH_META :meta

    . = ALIGN(4);
    _end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
