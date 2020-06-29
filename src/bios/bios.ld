ENTRY(_start)
MEMORY
{
    BIOS (rwx) :
        ORIGIN = 0xBFC00000,
        LENGTH = 0x10000

    BSS (rwx) :
        ORIGIN = 0x80020000,
        LENGTH = 0x1000
}

SECTIONS
{
    .text : {
        *(.bios.entry);
        . = 0x380;
        *(.bios.exception);
        . = 0x500;
        *(.text);
        *(.rodata);
        *(.data);
    } > BIOS

    .bss : {
        *(.bss);
    } > BSS

    /DISCARD/ : {
        *(.eh_frame);
        *(.MIPS.abiflags);
        *(.reginfo);
    }
}
