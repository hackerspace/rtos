#include <uart.h>

void sysputc(char mychar) {
  #define UART0_DATA ((volatile unsigned long *)(0x4000C000))
  #define UART0_FLAG ((volatile unsigned long *)(0x4000C018))
  while ((*UART0_FLAG & 0x20) != 0);
  *UART0_DATA = mychar;
  return;
}
