#include <sleep.h>
#include <c_entry.h>
#include <string.h>
#include <uart.h>

#define CPERIPH    0xE000E000
#define STCTRL     (CPERIPH+0x010)
#define STRELOAD   (CPERIPH+0x014)
#define STCURRENT  (CPERIPH+0x018)

#define NVIC       (CPERIPH+0x100)

#define INTCTRL    (0xe000ed04)

static unsigned int current_time;

typedef void (*fp)(void);
extern fp _reset;
extern int __memtop;

int tasks[4] = {0,0,0,0};
int _current_task = 0;
int _next_task = 0;
int task_count = 0;

void syscall(int a) {
}

void spawn_task(fp task) {
  tasks[0] = __memtop;
  ((int*)__memtop)[15] = (int)task;
}

void task1() {
  sysputc('1');
}

void systick(void) {
  current_time++;
  sysputc('K');

  // set PendSV and initiate context switch
  (*((int*)INTCTRL)) |= 1<<28;
}

void c_entry(void) {

  current_time = 0;

  *((int*)STRELOAD) = 10000;
  *((int*)STCURRENT) = 0;
  *((int*)STCTRL) |= 3;
  //*((int*)NVIC) |= (1<<15);

  //memcpy(0x0, tbl, sizeof(fp)*20);
  sysputc('W');

  spawn_task(task1);
}
