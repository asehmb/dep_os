
#include "../memory/memory.h"
#include "klib/kprintf.h"
#include "scheduler/scheduler.h"
#include <stddef.h>
#include <stdint.h>

void kmain() {

  asm volatile("msr daifclr, #2" ::: "memory"); // enable IRQ
  /* init_gic_cpu_interface(); */
  init_scheduler();
  init_mem();
  kprintf("Hi from kernel!\n");
  kprintf("Print Function!\n");
  kprintf("Null test\nIf this don't print, null terminator issue\n");
  kprintf("Integer test 1 (Expected 145): %d\n", 1455);
  kprintf("Integer test 2 (Expected 0): %d\n", 0);
  kprintf("Integer test 3 (Expected -1454): %d\n", -1454);
  kprintf("Integer test 4 (Expected 567, -1454): %d, %d\n", 567, -1454);
  kprintf("Char test 1 (Expected P): %c\n", 'P');
  kprintf("String test 1 (Expected Hello World!): %s\n", "Hello World!");

  kprintf("---- kprintf testing done! ----\n");

  while (1) {
    asm volatile("wfi");
  }
}
