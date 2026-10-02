

#include "../drivers/uart.h"
#include "../memory/memory.h"
#include "klib/kprintf.h"
#include "scheduler/scheduler.h"
#include "syscall.h"
#include <stddef.h>
#include <stdint.h>

volatile uint64_t g_user_return_elr = 0;

void force_fault() {
#define TRAP_BYTE 0x00 // On many architectures, 0x0000 is undefined or invalid
  void (*invalid_code)() = (void (*)()) "\x00\x00\x00\x00";
  invalid_code();
}

void kmain() {

  asm volatile("msr daifclr, #2" ::: "memory"); // enable IRQ
  kprintf("Hi From Kernel!\n");
  /* init_gic_cpu_interface(); */
  init_scheduler();
  init_mem();
  kprintf("Kernel Setup done!\n");

  while (1) {
    asm volatile("wfi");
  }
}
