.section .vectors, "ax"
.align 11
.global vectors

.extern handle_sync_exception
.extern handle_irq_exception
.extern handle_fiq_exception
.extern handle_serr_exception
.extern __exception_stack_top

.equ EXC_FRAME_SIZE, 288
.equ EXC_X0,         0
.equ EXC_X1,         8
.equ EXC_X2,        16
.equ EXC_X3,        24
.equ EXC_X4,        32
.equ EXC_X5,        40
.equ EXC_X6,        48
.equ EXC_X7,        56
.equ EXC_X8,        64
.equ EXC_X9,        72
.equ EXC_X10,       80
.equ EXC_X11,       88
.equ EXC_X12,       96
.equ EXC_X13,      104
.equ EXC_X14,      112
.equ EXC_X15,      120
.equ EXC_X16,      128
.equ EXC_X17,      136
.equ EXC_X18,      144
.equ EXC_X19,      152
.equ EXC_X20,      160
.equ EXC_X21,      168
.equ EXC_X22,      176
.equ EXC_X23,      184
.equ EXC_X24,      192
.equ EXC_X25,      200
.equ EXC_X26,      208
.equ EXC_X27,      216
.equ EXC_X28,      224
.equ EXC_X29,      232
.equ EXC_X30,      240
.equ EXC_SP,       248
.equ EXC_ELR,      256
.equ EXC_SPSR,     264
.equ EXC_ESR,      272
.equ EXC_FAR,      280

.macro SAVE_EXCEPTION_CONTEXT

    msr tpidr_el1, x0        // Stash x0
    msr sp_el0, x1           // Stash x1 (assumes we aren't using SP_EL0 right now)

    mov x0, sp               // x0 = Old SP
    ldr x1, =__exception_stack_top // x1 = New SP
    mov sp, x1
    sub sp, sp, #EXC_FRAME_SIZE
    stp x0, x1, [sp, #EXC_X0]

    stp x2, x3, [sp, #EXC_X2]
    stp x4, x5, [sp, #EXC_X4]
    stp x6, x7, [sp, #EXC_X6]
    stp x8, x9, [sp, #EXC_X8]
    stp x10, x11, [sp, #EXC_X10]
    stp x12, x13, [sp, #EXC_X12]
    stp x14, x15, [sp, #EXC_X14]
    stp x16, x17, [sp, #EXC_X16]
    stp x18, x19, [sp, #EXC_X18]
    stp x20, x21, [sp, #EXC_X20]
    stp x22, x23, [sp, #EXC_X22]
    stp x24, x25, [sp, #EXC_X24]
    stp x26, x27, [sp, #EXC_X26]
    stp x28, x29, [sp, #EXC_X28]
    str x30, [sp, #EXC_X30]

    // Save the stashed x0 and x1
    mrs x2, tpidr_el1
    mrs x3, sp_el0
    stp x2, x3, [sp, #EXC_X0]

    // Save the original SP (which we put in x0)
    str x0, [sp, #EXC_SP]

    mrs x9, elr_el1
    str x9, [sp, #EXC_ELR]
    mrs x9, spsr_el1
    str x9, [sp, #EXC_SPSR]
    mrs x9, esr_el1
    str x9, [sp, #EXC_ESR]
    mrs x9, far_el1
    str x9, [sp, #EXC_FAR]
.endm

.macro RESTORE_EXCEPTION_CONTEXT
    // Restore the original SP 
    ldr x0, [sp, #EXC_SP]
    msr sp_el0, x0 // move old sp to sp_el0

    ldp x0, x1, [sp, #EXC_X0]
    ldp x2, x3, [sp, #EXC_X2]
    ldp x4, x5, [sp, #EXC_X4]
    ldp x6, x7, [sp, #EXC_X6]
    ldp x8, x9, [sp, #EXC_X8]
    ldp x10, x11, [sp, #EXC_X10]
    ldp x12, x13, [sp, #EXC_X12]
    ldp x14, x15, [sp, #EXC_X14]
    ldp x16, x17, [sp, #EXC_X16]
    ldp x18, x19, [sp, #EXC_X18]
    ldp x20, x21, [sp, #EXC_X20]
    ldp x22, x23, [sp, #EXC_X22]
    ldp x24, x25, [sp, #EXC_X24]
    ldp x26, x27, [sp, #EXC_X26]
    ldp x28, x29, [sp, #EXC_X28]
    ldr x30, [sp, #EXC_X30]

    add sp, sp, #EXC_FRAME_SIZE
.endm

.macro EXCEPTION_HANDLER label, c_handler
\label:
    msr daifset, #2
    SAVE_EXCEPTION_CONTEXT
    mov x0, sp // stack pointer is the first argument to the C handler (frame pointer)
    bl \c_handler
    RESTORE_EXCEPTION_CONTEXT
    msr daifclr, #2 // enable interrupts (clear D bit in DAIF)
    eret
.endm

vectors:
    b sync_el1
    .space 0x7C
    b irq_el1
    .space 0x7C
    b fiq_el1
    .space 0x7C
    b serr_el1
    .space 0x7C

    b sync_el1
    .space 0x7C
    b irq_el1
    .space 0x7C
    b fiq_el1
    .space 0x7C
    b serr_el1
    .space 0x7C

    b sync_el1
    .space 0x7C
    b irq_el1
    .space 0x7C
    b fiq_el1
    .space 0x7C
    b serr_el1
    .space 0x7C

    b sync_el1
    .space 0x7C
    b irq_el1
    .space 0x7C
    b fiq_el1
    .space 0x7C
    b serr_el1
    .space 0x7C

EXCEPTION_HANDLER sync_el1, handle_sync_exception
EXCEPTION_HANDLER irq_el1, handle_irq_exception
EXCEPTION_HANDLER fiq_el1, handle_fiq_exception
EXCEPTION_HANDLER serr_el1, handle_serr_exception
