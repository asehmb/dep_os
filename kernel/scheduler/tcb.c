

#include "tcb.h"
#include "../../drivers/uart.h"

static uint8_t current_task = 0;
static struct task_struct tasks[MAX_TASKS];

void save_context(struct task_struct *task) {
  // Save the CPU context of the current thread into the task_struct
  asm volatile("msr daifset, #2\n"
               "stp x19, x20, [%0, #0]\n"
               "stp x21, x22, [%0, #16]\n"
               "stp x23, x24, [%0, #32]\n"
               "stp x25, x26, [%0, #48]\n"
               "stp x27, x28, [%0, #64]\n"
               "str fp, [%0, #80]\n" // Save frame pointer (x29)
               "str lr, [%0, #88]\n" // Save link register (x30)
               "mov x9, sp\n"        // Move stack pointer to x9 for storing
               "str x9, [%0, #96]\n" // Save stack pointer

               :
               : "r"(&task->cpu_context)
               : "memory");
}

void load_context(struct task_struct *task) {
  // Load the CPU context of the next thread from the task_struct
  asm volatile("ldp x19, x20, [%0, #0]\n"
               "ldp x21, x22, [%0, #16]\n"
               "ldp x23, x24, [%0, #32]\n"
               "ldp x25, x26, [%0, #48]\n"
               "ldp x27, x28, [%0, #64]\n"
               "ldr fp, [%0, #80]\n" // Load frame pointer (x29)
               "ldr lr, [%0, #88]\n" // Load link register (x30)
               "ldr x9, [%0, #96]\n" // Load stack pointer
               "mov sp, x9\n"        // Move loaded stack pointer to sp
               "msr daifset, #2\n"
               :
               : "r"(&task->cpu_context)
               : "memory");
}

struct task_struct *select_next_thread() {
  // For simplicity, we will just return a pointer to the next thread in a
  // round-robin manner. In a real implementation, you would have a more complex
  // scheduling algorithm and data structure.
  int next_task_index =
      (current_task++) &
      (MAX_TASKS - 1); // will eventually wrap around as it is unsigned
  uart_puts("Switching to task ");
  uart_putc('0' + next_task_index);
  return &tasks[next_task_index];
}

struct task_struct *get_current_thread() { return &tasks[current_task]; }
