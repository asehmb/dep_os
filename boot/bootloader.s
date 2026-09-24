    .section .text
    .global _start
    .extern __exception_stack_top

_start:
    msr     daifset, #0xf

    msr spsel, #1 // set stack to sp_el1
    ldr     x0, =__exception_stack_top
    mov     sp, x0
    
    ldr x0, =vectors
    msr vbar_el1, x0

    isb

    ldr     x0, =hello_world_string
    bl print_string

    isb

    bl kmain

hang:
    wfe
    b hang


    .equ UART0_BASE,  0x09000000
    .equ UARTDR,      0x00
    .equ UARTFR,      0x18
    .equ UARTFR_TXFF, 0x20

print_string:
1:
    ldrb    w1, [x0], #1
    cbz     w1, 2f

3:
    ldr     x2, =UART0_BASE
    ldr     w3, [x2, #UARTFR]
    tst     w3, #UARTFR_TXFF
    b.ne    3b

    str     w1, [x2, #UARTDR]
    b       1b

2:
    ret


    .section .rodata,"a"
hello_world_string:
    .asciz "Hello from bootloader!\n"



    .section .bss,"aw",@nobits
    .align  12


stack:
    .skip   4096
stack_top:


.section .page_tables, "aw", @nobits

.align 12
    // page tables


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
