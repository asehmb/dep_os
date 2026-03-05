

#include "../drivers/uart.h"
#include "scheduler/scheduler.h"
#include "syscall.h"
#include <stddef.h>
#include <stdint.h>

#define HEAP_START 0x5000000
#define HEAP_SIZE 0x10000
#define HEAP_CHUNK_SIZE 64

static uint8_t heap[HEAP_SIZE];
static uintptr_t heap_index = 0;
static uint8_t user_stack[4096] __attribute__((aligned(16)));

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

void *kmalloc(size_t size) {
  if (heap_index + size > HEAP_SIZE) {
    return NULL; // Out of memory
  }
  void *ptr = &heap[heap_index];
  if (heap_index & 0x7) {
    heap_index = (heap_index + 8) & ~0x7;
  }
  heap_index += size;
  return ptr;
}

void kmain() {
  init_gic_cpu_interface();
  init_scheduler();
  asm volatile("msr daifclr, #2" ::: "memory"); // enable IRQ
  uart_puts("Hi from kernel!\n");

  while (1) {
    asm volatile("wfi");
  }
}
