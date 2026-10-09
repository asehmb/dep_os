

#ifndef DEPOS_UART_H
#define DEPOS_UART_H

#define KERNEL_VIRT_BASE 0xFFFF800000000000
#define UART0_BASE 0x09000000 + KERNEL_VIRT_BASE
#define UARTDR (*(volatile unsigned int *)(UART0_BASE + 0x00))
#define UARTFR (*(volatile unsigned int *)(UART0_BASE + 0x18))
#define UARTFR_TXFF (1 << 5)

#include <stdint.h>

void uart_putc(char c);
void uart_getc(char *c);
void uart_gets(char *buf, int maxlen);
void uart_puts(const char *str);

void uart_print_hex(uint64_t val);

#endif // !DEBUG
