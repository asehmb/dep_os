    .section .text.boot
    .extern __exception_stack_top

.global _start
_start:
    msr daifset, #0xf
    msr spsel, #1


    // trying reading start of ram for dtb as x0 doesnt have it
    mov x1, #0x40000000
    mov x7, x1 // save x0, has device tree blob pointer
    
    // temp stack
    adrp x0, __exception_stack_top
    add  x0, x0, :lo12:__exception_stack_top
    mov  sp, x0

    // Build bootstrap tables, setup MAIR/TCR, and set TTBRx_EL1
    // setup mair_el1
    LDR X0, =0x00000000FF440400
    MSR mair_el1, x0

    // 48-bit Physical Address limit, Inner Shareable, Cacheable walks.
    LDR      X0, =0x5B5103510
    MSR      TCR_EL1, X0

    // setup boot tables
    adrp x0, boot_l0
    add  x0, x0, :lo12:boot_l0

    adrp x1, boot_l1
    add  x1, x1, :lo12:boot_l1

    orr x2, x1, #3
    
    str x2, [x0, #0]

    // index 256 (offset 2048) for 48 bit
    str x2, [x0, #2048]

    ldr x2, =0x00000401
    str x2, [x1, #0]
    ldr x2, =0x4000070D
    str x2, [x1, #8]

    // Setup TTBRx_EL1
    msr ttbr0_el1, x0
    msr ttbr1_el1, x0

    // cleanup before mmu enablement
    tlbi    vmalle1is
    dsb     ish
    isb

    // Enable the MMU
    mrs x0, sctlr_el1
    orr x0, x0, #1      // Enable MMU
    orr x0, x0, #(1<<2) // Enable D-Cache
    orr x0, x0, #(1<<12)// Enable I-Cache
    msr sctlr_el1, x0
    isb

    //  The Jump to the Higher Half
    ldr x0, =higher_half_entry
    br x0

higher_half_entry:

    ldr x0, =__exception_stack_top
    mov sp, x0
    isb
    
    ldr x0, =vectors
    msr vbar_el1, x0
    isb

    // store after mmu is ready
    adrp x0, dtb_ptr
    add x0, x0, :lo12:dtb_ptr
    str x7, [x0]



    bl kmain

// if fail to branch to main
hang:
    wfe
    b hang


    .section .rodata,"a"


    .section .bss,"aw",@nobits
    .align  12
    .global dtb_ptr
    dtb_ptr: .space 8



stack:
    .skip   4096
stack_top:


.section .page_tables, "aw", @nobits

.align 12
    // page tables
    boot_l0: .space 4096
    boot_l1: .space 4096

    .global l0_table
    .global l1_table
    .global l2_table
    .global l3_table
l0_table:
    .space 4096
l1_table:
    .space 4096
l2_table:
    .space 4096
l3_table:
    .space 4096
