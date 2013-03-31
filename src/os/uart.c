#include <system.h>
#include <uart.h>
#include <lm4f120h5qr.h>
#define GET32(x) (*(int*)(x))
#define PUT32(x,y) (*(int*)(x)) = y

#define GET16(x) (*(short*)(x))
#define PUT16(x,y) (*(short*)(x)) = y


#define RCCBASE 0x40023800
#define RCC_CR (RCCBASE+0x00)
#define RCC_PLLCFGR (RCCBASE+0x04)
#define RCC_CFGR (RCCBASE+0x08)
#define RCC_AHB1ENR (RCCBASE+0x30)
#define RCC_APB1ENR (RCCBASE+0x40)
#define GPIODBASE 0x40020C00
#define TIM5BASE 0x40000C00
#define FLASH_ACR 0x40023C00
#define GPIOABASE 0x40020000
#define GPIOA_MODER (GPIOABASE+0x00)

#ifdef STM32

#define USART2_BASE 0x40004400
#define USART2_SR (USART2_BASE+0x00)
#define USART2_DR (USART2_BASE+0x04)
#define USART2_BRR (USART2_BASE+0x08)
#define USART2_CR1 (USART2_BASE+0x0C)
#define USART2_CR2 (USART2_BASE+0x10)
#define USART2_CR3 (USART2_BASE+0x14)
#define USART2_GTPR (USART2_BASE+0x18)
#define GPIOA_AFRL (GPIOABASE+0x20)
#define GPIOA_OTYPER (GPIOABASE+0x04)

#else

#define USART2_BASE 0x4000e000
#define USART2_SR (USART2_BASE+0x04)
#define USART2_DR (USART2_BASE+0x00)
#define USART2_BRR (USART2_BASE+0x08)
#define USART2_CR1 (USART2_BASE+0x0C)
#define USART2_CR2 (USART2_BASE+0x10)
#define USART2_CR3 (USART2_BASE+0x14)
#define USART2_GTPR (USART2_BASE+0x18)
#define GPIOA_AFRL (GPIOABASE+0x20)
#define GPIOA_OTYPER (GPIOABASE+0x04)

#endif

#define _buf_max  64
#define _uarts  2

typedef struct {
  char buf[_buf_max];
  char cnt;
  char top;
  char bot;
  char echo;
} ring_buf;
ring_buf _buf[2]; //two uarts

int uart_b_tx = 0;
int uart_b_rx = 0;

int uart_init(void) {
  for (int i = 0; i < _uarts; i++) {
    _buf[i].cnt = 0;
    _buf[i].top = 0;
    _buf[i].bot = 0;
    _buf[i].echo = 0;
  }

  UART0_CTL_R &= ~0b1;
//  UART0_ICR_R |= 1<<4;
  UART0_IM_R |= 1<<4;
  UART0_CTL_R |= 0b11 << 8; //rxe, txe
  UART0_CTL_R |= 0b1;

  UART1_CTL_R &= ~0b1;
//  UART1_ICR_R |= 1<<4;
  UART1_IM_R |= 1<<4;
  UART1_CTL_R |= 0b11 << 8; //rxe, txe
  UART1_CTL_R |= 0b1;

#ifdef LM4F
  SYSCTL_RCGCUART_R |= 0b10; // clock gate to UART1
  SYSCTL_RCGCGPIO_R |= 0b10; // clock gate to port B
  GPIO_PORTB_AFSEL_R |= 0b11;
  GPIO_PORTB_DEN_R  |= 0b00;
  GPIO_PORTB_DIR_R  |= 0b00;
//  GPIO_PORTB_ODR_R  &= ~0b11;
  GPIO_PORTB_PUR_R |= 0b00;
  GPIO_PORTB_PCTL_R |= 0x1;
  UART1_CTL_R &= ~0b1; // disable uart1
  UART1_CTL_R &= ~(0b11 << 8); //rxe, txe

  UART1_IBRD_R = 10; // from datasheet p.861
  UART1_FBRD_R = 54;
  UART1_LCRH_R = 0x60;
  UART1_CC_R   = 0x0; // system clock
  // no dma
  // and enable

  UART1_CTL_R |= 0b11 << 8; //rxe, txe
  UART1_CTL_R |= 0b1; // uarten
//  GPIO_PORTB_DEN_R  |= 0b11;
//  UART0_CTL_R |= 0b1;
#endif
}

//-------------------------------------------------------------------
int uart_init_ ( void )
{
    unsigned int ra;

    ra=GET32(RCC_AHB1ENR);
    ra|=1<<0; //enable port A
    PUT32(RCC_AHB1ENR,ra);

    ra=GET32(RCC_APB1ENR);
    ra|=1<<17; //enable USART2
    PUT32(RCC_APB1ENR,ra);

    //PA2 USART2_TX
    //PA3 USART2_RX

    ra=GET32(GPIOA_MODER);
    ra|= (2<<4);
    ra|= (2<<6);
    PUT32(GPIOA_MODER,ra);
    ra=GET32(GPIOA_OTYPER);
    ra&=(1<<2);
    ra&=(1<<3);
    PUT32(GPIOA_OTYPER,ra);
    ra=GET32(GPIOA_AFRL);
    ra|=(7<<8);
    ra|=(7<<12);
    PUT32(GPIOA_AFRL,ra);

    //42000000/16 = 2625000
    //2625000/115200 = 22.786...
    //.786... * 16 = 12.58
    //12/16 = 0.75
    //13/16 = 0.8125
    //2625000/22.75 = 115384 184 .16%
    //2625000/22.8125 = 115068 132 .11%
    PUT32(USART2_BRR,(22<<4)|(13<<0));
    PUT32(USART2_CR1,(1<<13)|(1<<3)|(1<<2));
    return(0);
}
/*
void uart_putc ( unsigned int x ) {
    while (( GET32(USART2_SR) & (1<<7)) == 0) continue;
    PUT32(USART2_DR,x);
}
unsigned int uart_getc ( void ) {
    while (( GET32(USART2_SR) & (1<<5)) == 0) continue;
    return(GET32(USART2_DR));
}*/



//void sysputc___(char mychar) {uart_putc(mychar);};

//void sysputc_(char mychar) {toggle_led(0b100);};

//void sysputc__(char c) {};

#include <stdio.h>
void print_num(int num);
void term_goto(int x, int y);


//#ifdef LM4F
  #define UART0_DATA ((volatile unsigned long *)(0x4000c000))
  #define UART0_FLAG ((volatile unsigned long *)(0x4000c018))
  #define UART1_DATA ((volatile unsigned long *)(0x4000d000))
  #define UART1_FLAG ((volatile unsigned long *)(0x4000d018))
/*#else
//for qemu, really it's uart0
  #define UART1_DATA ((volatile unsigned long *)(0x4000C004))
  #define UART1_FLAG ((volatile unsigned long *)(0x4000C000))
#endif*/


void rbuf_put(int id, int what) {
  if (_buf[id].cnt < _buf_max) {
    _buf[id].buf[_buf[id].top] = (char)what;
    _buf[id].top++;
    if (_buf[id].top >= _buf_max) {
      _buf[id].top = 0;
    }
    _buf[id].cnt++;
  }
//  sysputc(x);
}

void uart1_interrupt() {
//  term_goto(10,10);
  
  int d = sysgetc1();
  if (_buf[1].echo) {
    if (d == '\n')
      sysputc1('\r');
    sysputc1(d);
  }
  rbuf_put(1, d);

}


void uart0_interrupt() {
  int d = sysgetc();
  rbuf_put(0, d);
  if (_buf[0].echo) {
    if (d == '\n')
      sysputc('\r');
    sysputc(d);
  }
//  UART0_ICR_R |= 1<<4;
//  term_goto(12,10);
}


void sysputc1(char mychar) {
  while ((*UART1_FLAG & 0x20) != 0);
  *UART1_DATA = mychar;
  uart_b_tx++;
  return;
}

int sysgetc1(void) {
  while ((*UART1_FLAG & (1<<4)) != 0);
  int d = *UART1_DATA;
  uart_b_rx++;
  return d;
}

void sysputc(char mychar) {
  while ((*UART0_FLAG & 0x20) != 0);
  *UART0_DATA = mychar;
  uart_b_tx++;
  return;
}

int sysgetc(void) {
  while ((*UART0_FLAG & (1<<4)) != 0);
  int x = *UART0_DATA;
  uart_b_rx++;
  return x;
}

int rbuf_get(int id) {
  //if (!_buf[id].cnt) return 0;
  while (!_buf[id].cnt) {
    asm volatile ("wfi");
  };
  int x = _buf[id].buf[_buf[id].bot];
  _buf[id].bot++;
  if (_buf[id].bot >= _buf_max) {
    _buf[id].bot = 0;
  }
  _buf[id].cnt--;
  return x;
}

//0 - uart0
//1 - uart1
int uart_getc(int dev) {
  return rbuf_get(dev);
}

int uart_notempty(int dev) {
  return _buf[dev].cnt;
}

int uart_setecho(int dev, int enable) {
  _buf[dev].echo = enable;
}
