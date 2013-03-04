#include <sleep.h>
#include <c_entry.h>
#include <string.h>


#define CPERIPH    0xE000E000
#define STCTRL     (CPERIPH+0x010)
#define STRELOAD   (CPERIPH+0x014)
#define STCURRENT  (CPERIPH+0x018)

#define NVIC       (CPERIPH+0x100)

static unsigned int current_time;

typedef void (*fp)(void);
extern fp _reset;

void systick(void) {
  current_time++;
}

void c_entry(void) {
  //fp tbl[20] = {[0 ... 19] = sys_sleep};
  //fp tbl[20];
  //fp *tbl = (fp*)0x0;

  //tbl[1] = sys_sleep;
  /*
  tbl = (fp)0x00004000;
  tbl[1] = _reset;

  tbl[15] = systick;
  */

  current_time = 0;
  //asm volatile ("CPSIE i");

  *((int*)STRELOAD) = 10000;
  *((int*)STCURRENT) = 0;
  *((int*)STCTRL) |= 3;
  //*((int*)NVIC) |= (1<<15);

  //memcpy(0x0, tbl, sizeof(fp)*20);
}
