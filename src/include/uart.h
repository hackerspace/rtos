#ifndef _UART_H_
#define _UART_H_

void sysputc(char mychar);
int sysgetc(void);
void sysputc1(char mychar);
int sysgetc1(void);
int uart_init(void);
int uart_getc(int);

int uart_notempty(int dev);
int uart_setecho(int dev, int enable);

#endif
