
#include "uart.h"

void uart_putc(char c) {
  /* Wait until TX FIFO is not full */
  while (UARTFR & UARTFR_TXFF)
    ;

  /* Write character to data register */
  UARTDR = c;
}

void uart_getc(char *c) {
  /* Wait until RX FIFO is not empty */
  while (UARTFR & (1 << 4))
    ;

  /* Read character from data register */
  *c = (char)(UARTDR & 0xFF);
}

void uart_gets(char *buf, int maxlen) {
  int i = 0;
  char c;
  while (i < maxlen - 1) {
    uart_getc(&c);
    if (c == '\n' || c == '\r') {
      break;
    }
    buf[i++] = c;
  }
  buf[i] = '\0';
}

void uart_puts(const char *str) {
  while (*str) {
    uart_putc(*str++);
  }
}

void uart_print_hex(uint64_t val) {
  uart_puts("0x");
  for (int i = 60; i >= 0; i -= 4) {
    uint8_t nibble = (val >> i) & 0xF;
    uart_putc(nibble < 10 ? '0' + nibble : 'A' + (nibble - 10));
  }
}
