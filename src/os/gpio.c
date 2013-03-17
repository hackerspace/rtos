#include <gpio.h>

#include <lm4f120h5qr.h>

void gpio_init_lm4f();
void gpio_init_stm32();


void toggle_led(int id) {
  id &= 0b101;
  GPIO_PORTF_DATA_R ^= (id << 1);
}


void gpio_init(void) {
//  gpio_init_stm32();
  gpio_init_lm4f();
}

void gpio_init_lm4f(void) {
  SYSCTL_RCGCGPIO_R |= 0x20; // clock gate to port F
  GPIO_PORTF_DIR_R |= 0b11111110; //0x7 << 1; // Set Led's as outputs
  GPIO_PORTF_DEN_R |= 0b11111110; //0x7 << 1; // for LED's

  GPIO_PORTF_DATA_R = 0;

  toggle_led(0b010);


  SYSCTL_RCGCGPIO_R |= 0X1; // clock gate to port F
  //GPIO_PORTA_DIR_R = 0b0; // PA2 as input
  //GPIO_PORTA_DEN_R = 0b100; // PA2

  //GPIO_PORTA_ICR_R |= 0b100;

}

void gpio_init_stm32(void) {

/*
// enable PORT F GPIO peripheral
  SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
  // set LED PORT F pins as outputs
  GPIO_PORTF_DIR_R = LED_RED|LED_BLUE|LED_GREEN;
  // enable digital for LED PORT F pins
  GPIO_PORTF_DEN_R = LED_RED|LED_BLUE|LED_GREEN;
  // clear all PORT F pins
  GPIO_PORTF_DATA_R = 0;
  // set LED PORT F pins high
  GPIO_PORTF_DATA_R |= LED_RED|LED_BLUE|LED_GREEN;
  // loop forever
*/

/*    uart_init ();

    int ra=GET32(RCCBASE+0x30);
    ra|=1<<3; //enable port D
    PUT32(RCCBASE+0x30,ra);

    //d12 = d15 output
    ra=GET32(GPIODBASE+0x00);
    ra&=0x00FFFFFF;
    ra|=0x55000000;
    PUT32(GPIODBASE+0x00,ra);
    //push pull
    ra=GET32(GPIODBASE+0x04);
    ra&=0xFFFF0FFF;
    PUT32(GPIODBASE+0x04,ra);

    for(int rx=0;rx<1;rx++)
    {
        PUT32(GPIODBASE+0x18,0xE0001000);
        for(ra=0;ra<200000;ra++) continue;
        PUT32(GPIODBASE+0x18,0xD0002000);
        for(ra=0;ra<200000;ra++) continue;
        PUT32(GPIODBASE+0x18,0xB0004000);
        for(ra=0;ra<200000;ra++) continue;
        PUT32(GPIODBASE+0x18,0x70008000);
        for(ra=0;ra<200000;ra++) continue;
    } */


}
