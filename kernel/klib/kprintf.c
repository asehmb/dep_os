

#include "kprintf.h"
#include "../../drivers/uart.h"
#include <stdarg.h>
#include <stdint.h>

void _kprintf_testing() {
  kprintf("----Print Function test----\n");
  kprintf("Null test\nIf this don't print, null terminator issue\n");
  kprintf("Integer test 1 (Expected 145): %d\n", 1455);
  kprintf("Integer test 2 (Expected 0): %d\n", 0);
  kprintf("Integer test 3 (Expected -1454): %d\n", -1454);
  kprintf("Integer test 4 (Expected 567, -1454): %d, %d\n", 567, -1454);
  kprintf("Char test 1 (Expected P): %c\n", 'P');
  kprintf("String test 1 (Expected Hello World!): %s\n", "Hello World!");
  kprintf("---- kprintf testing done! ----\n");
}

int kprintf(const char *text, ...) {
  va_list ap;
  va_start(ap, text);

  const char *schar = text;

  while (*schar != '\0') {

    if (*schar == '%') {
      schar++;
      if (*schar == 'd') {
        // get each digit and typecast to char
        // uart_putc
        int i = va_arg(ap, int);
        char int_array[20] = {0}; // max of 20 digits in 64bit
        uint8_t current_digit = 0;
        if (i == 0) {
          uart_putc('0');
        }
        if (i < 0) {
          uart_putc('-');
          i = -i;
        }
        while (i != 0) {
          int_array[current_digit] = (char)((i % 10) + '0');
          i /= 10;
          current_digit++;
        }
        for (i = current_digit - 1; i >= 0; i--) {
          uart_putc(int_array[i]);
        }
        schar++;
      } else if (*schar == 'c') {
        int c = va_arg(ap, int);
        uart_putc((char)c);
        schar++;
      } else if (*schar == 's') {
        char *string = va_arg(ap, char *);
        uart_puts(string);
        schar++;
      } else if (*schar == 'x') {
        uint64_t hex = va_arg(ap, int);
        uart_print_hex(hex);
        schar++;
      }
    }
    uart_putc(*schar);
    schar += 1;
  }

  va_end(ap);

  return 0;
}
