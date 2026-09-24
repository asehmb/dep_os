

#include "../drivers/uart.h"
#include "../memory/memory.h"
#include "scheduler/scheduler.h"
#include "syscall.h"
#include <stddef.h>
#include <stdint.h>

volatile uint64_t g_user_return_elr = 0;

static void init_gic_cpu_interface(void) {
  volatile uint32_t *gicd_ctlr = (volatile uint32_t *)0x08000000;
  volatile uint32_t *gicd_igroupr0 = (volatile uint32_t *)0x08000080;
  volatile uint32_t *gicd_isenabler0 = (volatile uint32_t *)0x08000100;
  volatile uint8_t *gicd_ipriorityr = (volatile uint8_t *)0x08000400;

  volatile uint32_t *gicc_ctlr = (volatile uint32_t *)0x08010000;
  volatile uint32_t *gicc_pmr = (volatile uint32_t *)0x08010004;
  volatile uint32_t *gicc_bpr = (volatile uint32_t *)0x08010008;
  const uint32_t timer_ppi_mask = (1U << 27);

  *gicd_ctlr = 0;
  *gicc_ctlr = 0;

  gicd_ipriorityr[27] = 0x10;
  *gicd_igroupr0 |= timer_ppi_mask;
  *gicd_isenabler0 = timer_ppi_mask;

  *gicc_pmr = 0xFF;
  *gicc_bpr = 0;
  *gicc_ctlr = 3;
  *gicd_ctlr = 3;
  asm volatile("dsb sy");
  asm volatile("isb");
}

void kmain() {

  asm volatile("msr daifclr, #2" ::: "memory"); // enable IRQ
  /* init_gic_cpu_interface(); */
  init_scheduler();
  init_mem();
  uart_puts("Hi from kernel!\n");

  while (1) {
    asm volatile("wfi");
  }
}
