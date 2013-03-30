#ifndef __GPIO__H__
#define __GPIO__H__

void gpio_init(void);

void toggle_led(int id);

void gpio_interrupt();
#endif
