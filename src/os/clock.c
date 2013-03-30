#include <clock.h>

#define GET32(x) (*(int*)(x))
#define PUT32(x,y) (*(int*)(x)) = y

#define GET16(x) (*(short*)(x))
#define PUT16(x,y) (*(short*)(x)) = y

/*
void PUT32 ( unsigned int, unsigned int );
void PUT16 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
unsigned int GET16 ( unsigned int );
*/



/*
//main osc ctl
#define MOSCCTL 0x400FE07c
#define RCC 0x400fe060
// 16mhz
#define XTAL 0x15
    int rcc = GET32(RCC);
    rcc |= XTAL << 6;
    rcc &= ~(1 << 11); //turn off PLL bypass so we can run fast
    rcc &= ~(2 << 4); //set Main OSC as clock
    rcc &= 1; // Main OSC enable
    PUT32(RCC, rcc);
    PUT32(MOSCCTL, 0);

#define RCC2 (*(int*)(0x400FE070))
    RCC2 &= ~(0b000 << 4); //oscrc2 ten mosc as main osc
*/


#include <lm4f120h5qr.h>
void ClockInit_lm4f(void) {
  // step 1
  SYSCTL_RCC_R &= ~(SYSCTL_RCC_USESYSDIV);
  SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;

  // step 2
  SYSCTL_RCC_R &= ~(SYSCTL_RCC_XTAL_M);
  SYSCTL_RCC_R |= SYSCTL_RCC_XTAL_16MHZ;

  SYSCTL_RCC_R &= ~(SYSCTL_RCC_OSCSRC_M);
  SYSCTL_RCC_R |= SYSCTL_RCC_OSCSRC_MAIN;

  SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_USERCC2);

  SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_OSCSRC2_M);
  SYSCTL_RCC2_R |= SYSCTL_RCC2_OSCSRC2_MO;

  SYSCTL_RCC_R &= ~(SYSCTL_RCC_PWRDN);
  SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_PWRDN2);

  // step 3
  SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_SYSDIV2_M);
  SYSCTL_RCC2_R |= SYSCTL_RCC2_SYSDIV2_2;

//  SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;
  SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_SYSDIV2LSB);
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;

  // step 4
  while (!(SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS));
  // step 5
  SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;
  SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_BYPASS2);
//  SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;
//  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;

}

#define RCCBASE 0x40023800
#define RCC_CR (RCCBASE+0x00)
#define RCC_PLLCFGR (RCCBASE+0x04)
#define RCC_CFGR (RCCBASE+0x08)
#define GPIODBASE 0x40020C00
#define TIM5BASE 0x40000C00
#define FLASH_ACR 0x40023C00

void ClockInit_stm32 ( void )
{
    unsigned int ra;

    //enable HSE
    ra=GET32(RCC_CR);
    ra&=~(0xF<<16);
    PUT32(RCC_CR,ra);
    ra|=1<<16;
    PUT32(RCC_CR,ra);
    while(1)
    {
        if(GET32(RCC_CR)&(1<<17)) break;
    }
    PUT32(RCC_CFGR,0x00009401); //PPRE2 /2 PPRE1 /4 sw=hse

    //slow flash accesses down otherwise it will crash
    PUT32(FLASH_ACR,0x00000105);

    //8MHz HSE, 168MHz pllgen 48MHz pll usb
    //Q 7 P 2 N 210 M 5 vcoin 1 pllvco 336 pllgen 168 pllusb 48
    ra=(7<<24)|(1<<22)|(((2>>1)-1)<<16)|(210<<6)|(5<<0);
    PUT32(RCC_PLLCFGR,ra);

    // enable pll
    ra=GET32(RCC_CR);
    ra|=(1<<24);
    PUT32(RCC_CR,ra);

    //wait for pll lock
    while(1)
    {
        if(GET32(RCC_CR)&(1<<25)) break;
    }

    //select pll
//    PUT32(RCC_CFGR,0x00009402); //PPRE2 /2 PPRE1 /4 sw=pllclk
  //select pll
    PUT32(RCC_CFGR,0x0000B402); //PPRE2 /4 PPRE1 /4 sw=pllclk

    //if you didnt set the flash wait states you may crash here

    //wait for it to use the pll
    while(1)
    {
        if((GET32(RCC_CFGR)&0xC)==0x8) break;
    }
 }



void ClockInit(void) {
  //ClockInit_stm32();
  ClockInit_lm4f();
}
