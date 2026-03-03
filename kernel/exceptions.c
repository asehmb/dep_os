#include "../drivers/uart.h"
#include <stdint.h>

struct exception_frame {
  uint64_t x[31];
  uint64_t sp;
  uint64_t elr_el1;
  uint64_t spsr_el1;
  uint64_t esr_el1;
  uint64_t far_el1;
};

static void uart_put_hex64(uint64_t value) {
  static const char hex[] = "0123456789ABCDEF";
  uart_puts("0x");
  for (int shift = 60; shift >= 0; shift -= 4) {
    uart_putc(hex[(value >> shift) & 0xF]);
  }
}

static void dump_register(const char *name, uint64_t value) {
  uart_puts(name);
  uart_puts(": ");
  uart_put_hex64(value);
  uart_puts("\n");
}

static void dump_all_registers(struct exception_frame *frame) {
  dump_register("x0", frame->x[0]);
  dump_register("x1", frame->x[1]);
  dump_register("x2", frame->x[2]);
  dump_register("x3", frame->x[3]);
  dump_register("x4", frame->x[4]);
  dump_register("x5", frame->x[5]);
  dump_register("x6", frame->x[6]);
  dump_register("x7", frame->x[7]);
  dump_register("x8", frame->x[8]);
  dump_register("x9", frame->x[9]);
  dump_register("x10", frame->x[10]);
  dump_register("x11", frame->x[11]);
  dump_register("x12", frame->x[12]);
  dump_register("x13", frame->x[13]);
  dump_register("x14", frame->x[14]);
  dump_register("x15", frame->x[15]);
  dump_register("x16", frame->x[16]);
  dump_register("x17", frame->x[17]);
  dump_register("x18", frame->x[18]);
  dump_register("x19", frame->x[19]);
  dump_register("x20", frame->x[20]);
  dump_register("x21", frame->x[21]);
  dump_register("x22", frame->x[22]);
  dump_register("x23", frame->x[23]);
  dump_register("x24", frame->x[24]);
  dump_register("x25", frame->x[25]);
  dump_register("x26", frame->x[26]);
  dump_register("x27", frame->x[27]);
  dump_register("x28", frame->x[28]);
  dump_register("x29", frame->x[29]);
  dump_register("x30", frame->x[30]);

  dump_register("sp", frame->sp);
  dump_register("elr_el1", frame->elr_el1);
  dump_register("spsr_el1", frame->spsr_el1);
  dump_register("esr_el1", frame->esr_el1);
  dump_register("far_el1", frame->far_el1);
}

static void handle_exception(const char *name, struct exception_frame *frame) {
  uart_puts("\n");
  uart_puts(name);
  uart_puts(" exception\n");
  dump_all_registers(frame);
}

void kernel_panic(struct exception_frame *frame) {
  uart_puts("\nKernel Panic!\n");
  dump_all_registers(frame);
  while (1) {
    // Halt the system
    asm volatile("wfe"); // Wait For Event - low power until next interrupt
  }
}

void handle_sync_exception(struct exception_frame *frame) {
  uint32_t ec = (frame->elr_el1 >> 26) & 0x3F;

  if (ec == 0x15) { // 0x15 is the code for an "SVC instruction"
    uart_puts("System Call Triggered!\n");

    // Move the return address to the NEXT instruction
    frame->elr_el1 += 4;
    return; // This returns to assembly, which calls RESTORE and eret
  } else {
    uart_puts("Unknown Sync Exception (EC=");
    uart_put_hex64(ec);
    // It's a real crash (Page Fault, etc.)
    kernel_panic(frame);
  }
}

void handle_irq_exception(struct exception_frame *frame) {
  handle_exception("IRQ", frame);
}

void handle_fiq_exception(struct exception_frame *frame) {
  handle_exception("FIQ", frame);
}

void handle_serr_exception(struct exception_frame *frame) {
  handle_exception("SError", frame);
}
