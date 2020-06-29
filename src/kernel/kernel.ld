ENTRY(_start)
MEMORY
{
    SEG1 (rwx) :
        ORIGIN = 0x80000000,
        LENGTH = 0x20000
    SEG2 (rwx) :
        ORIGIN = 0x80030000,
        LENGTH = 0x10000
    SEG3 (rwx) :
        ORIGIN = 0x80018000,
        LENGTH = 0x8000
}

SECTIONS
{
    .text : {
        . = 0x180;
        *(.exception);
        . = 0x500;
        *(.entry);
        *(.text);
        *(.rodata);
        *(.text.obf);
        *(.data);
        *(.vm.code);
    } > SEG1

    .rodata : {
        *(.data.obf);
    } > SEG2

    .bss : {
        *(.bss);
        . = 0x8000;
        PROVIDE(VM_SP_TOP = .);
    } > SEG3
}
