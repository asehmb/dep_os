

#ifndef DEPOS_UART_H
#define DEPOS_UART_H

#define UART0_BASE 0x09000000
#define UARTDR (*(volatile unsigned int *)(UART0_BASE + 0x00))
#define UARTFR (*(volatile unsigned int *)(UART0_BASE + 0x18))
#define UARTFR_TXFF (1 << 5)

void uart_putc(char c);
void uart_getc(char *c);
void uart_gets(char *buf, int maxlen);
void uart_puts(const char *str);

#endif // !DEBUG
