
#include "uart.h"
#define UART0_BASE 0x09000000
#define UARTDR (*(volatile unsigned int *)(UART0_BASE + 0x00))
#define UARTFR (*(volatile unsigned int *)(UART0_BASE + 0x18))
#define UARTFR_TXFF (1 << 5)

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
