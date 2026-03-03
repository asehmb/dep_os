

#include "../drivers/uart.h"
#include <stddef.h>
#include <stdint.h>

#define HEAP_START 0x5000000
#define HEAP_SIZE 0x10000
#define HEAP_CHUNK_SIZE 64

static uint8_t heap[HEAP_SIZE];
static uintptr_t heap_index = 0;

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

  uart_puts("Hi from kernel!\n\0");
  char *buffer = (char *)kmalloc(128);
  volatile int a = 10;
  volatile int b = 0;
  int c = a / b; // Triggers #DE (Divide Error, Vector 0)

  while (1) {
    asm volatile("wfe");
    uart_puts("Input: \0");
    uart_gets(buffer, sizeof(buffer));
    uart_puts("\nYou typed: \0");
    uart_puts(buffer);
    uart_puts("\n\0");
    if (buffer[0] == 'q') {
      break;
    }
  }
}
