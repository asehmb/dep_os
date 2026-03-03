
ENTRY(_start)

SECTIONS {
    . = 0x40000000;

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
        . = ALIGN(16);
        __exception_stack_bottom = .;
        . = . + 0x4000;
        __exception_stack_top = .;
    }
}
