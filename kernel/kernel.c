

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
  asm volatile("msr daifclr, #2" ::: "memory"); // enable interrupts
  init_scheduler();
  uart_puts("Hi from kernel!\n");

  while (1) {
    asm volatile("wfe");
  }
}
