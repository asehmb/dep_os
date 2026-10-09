
ENTRY(_start)
KERNEL_VIRT_BASE = 0xFFFF800000000000;
KERNEL_PHYS_BASE = 0x41000000;

SECTIONS {
    . = KERNEL_PHYS_BASE + KERNEL_VIRT_BASE;

    .text : AT(ADDR(.text) - KERNEL_VIRT_BASE){
        *(.text.boot)
        *(.text*)
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_VIRT_BASE){
        *(.rodata*)
    }

    .bss : AT(ADDR(.bss) - KERNEL_VIRT_BASE){
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
    .page_tables (NOLOAD) : AT(ADDR(.page_tables) - KERNEL_VIRT_BASE) {
        *(.page_tables)
    }
    __page_tables_end = .;

    /* Mark the absolute end of the loaded kernel image */
    . = ALIGN(4096);

    _kernel_end = .;
}
