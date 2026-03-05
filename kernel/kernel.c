

#include "../drivers/uart.h"
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

static long user_syscall_write(const char *buf, uint64_t count) {
  register uint64_t x0 asm("x0") = 1;
  register const char *x1 asm("x1") = buf;
  register uint64_t x2 asm("x2") = count;
  register uint64_t x8 asm("x8") = SYSCALL_WRITE;

  asm volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x8) : "memory");
  return (long)x0;
}

static long user_syscall_exit(int status) {
  register uint64_t x0 asm("x0") = (uint64_t)status;
  register uint64_t x8 asm("x8") = SYSCALL_EXIT;

  asm volatile("svc #0" : "+r"(x0) : "r"(x8) : "memory");
  return (long)x0; // This line will never be reached
}

static void user_main(void) {
  static const char msg[] = "Hi from user via syscall!\n";
  (void)user_syscall_write(msg, sizeof(msg) - 1);
  (void)user_syscall_exit(0);

  /* while (1) { */
  /*   asm volatile("wfe"); */
  /* } */
}

static void enter_user_mode(void (*entry)(void), uint64_t user_sp) {
  asm volatile("msr sp_el0, %0\n"
               "msr elr_el1, %1\n"
               "mov x2, #0\n"
               "msr spsr_el1, x2\n"
               "eret\n"
               :
               : "r"(user_sp), "r"(entry) // pass user_sp in x0 and entry in x1
               : "x2", "memory");         // clobber x2 and memory
}

void kmain() {
  uart_puts("Hi from kernel!\n");
  g_user_return_elr = (uint64_t)(uintptr_t)&&return_from_user;
  enter_user_mode(user_main,
                  (uint64_t)(uintptr_t)(user_stack + sizeof(user_stack)));

return_from_user:
  uart_puts("Back in kernel after user syscall.\n");
  while (1) {
    asm volatile("wfe");
  }
}
