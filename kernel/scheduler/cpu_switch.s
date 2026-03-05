
.section .text
// void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
.global cpu_switch
cpu_switch:
    // Save previous task
    stp x19, x20, [x0], #16     // Store x19, x20 and increment pointer by 16 bytes
    stp x21, x22, [x0], #16
    stp x23, x24, [x0], #16
    stp x25, x26, [x0], #16
    stp x27, x28, [x0], #16
    stp x29, x30, [x0], #16     // Save FP (x29) and LR (x30)
    
    mov x9, sp                  // Get current stack pointer
    str x9, [x0]                // Save SP to the context

    // Restore the next task's state (next is in x1)
    ldp x19, x20, [x1], #16     // Load x19, x20 from next task's context
    ldp x21, x22, [x1], #16
    ldp x23, x24, [x1], #16
    ldp x25, x26, [x1], #16
    ldp x27, x28, [x1], #16
    ldp x29, x30, [x1], #16     // Restore FP and LR
    
    ldr x9, [x1]                // Load next task's stack pointer
    mov sp, x9                  // Apply it to SP

    ret                         // Return (using the restored x30 Link Register)

