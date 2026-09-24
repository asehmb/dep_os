
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

    /* Reserve 4KB-aligned space for early boot page tables */
    . = ALIGN(4096);
    __page_tables_start = .;
    .page_tables (NOLOAD) : {
        . += 4096 * 4; /* Space for 3 tables (L0, L1, L2 = 12KB) */
    }
    __page_tables_end = .;

    /* Mark the absolute end of the loaded kernel image */
    . = ALIGN(4096);
    _kernel_end = .;
}
