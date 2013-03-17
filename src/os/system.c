#include <sleep.h>
#include <system.h>
#include <string.h>
#include <uart.h>
#include <clock.h>
#include <stdio.h>
#include <gpio.h>

#define RCCBASE   0x40023800
#define GPIODBASE 0x40020C00
#define GET32(x) (*(int*)(x))

#define PUT32(x,y) (*(int*)(x)) = y


#define CPERIPH    0xE000E000
#define STCTRL     (CPERIPH+0x010)
#define STRELOAD   (CPERIPH+0x014)
#define STCURRENT  (CPERIPH+0x018)

#define NVIC       (CPERIPH+0x100)

#define INTCTRL    (0xe000ed04)

#include <lm4f120h5qr.h>

extern void _systick();
extern void _svcall();
extern void _ctx_switcher();
static unsigned int current_time;

typedef void (*fp)(void);
extern fp _reset;
int _memtop;
extern int __memtop;

struct stack_frame *tasks[4];
int _current_task = 0;
int _next_task = 0;
int task_count = 0;
int _first_time = 0;

int __heap_top;
int __heap_start;

void syscall(int a) {
  sysputc(_current_task+'1');
}

void spawn_task(fp task) {
  struct stack_frame *s_frm =
    (struct stack_frame *)(_memtop - sizeof(struct stack_frame));

  s_frm->pc = (uint32_t)(task-1);
  //where to jump
  s_frm->psr = 0x21000000;
  //default program status word

  tasks[task_count] = s_frm;
  //save pointer to top of tasks stack
  tasks[task_count] -= 1;
  //move 'one stack' down  //sfrm_size*sizeof(int);

  task_count++;
  _memtop -= 0x100;
}

void task1() {
  int a;

  while(1) {
    a=0;
    while(a++!=1000);
    toggle_led(0b100);
//    sysputc('~');
  }
}

void task2() {
  int a;

  while(1) {
    a=0;
    while(a++!=1000);
    toggle_led(0b001);
    //sysputc('!');
  }
}

void context_sw(void) {
  _current_task = _next_task;
  _first_time = 0;
  toggle_led(0b100);
  sysputc('C');
}

void systick(void) {
  if (!_first_time) {
    asm volatile (\
      "mrs r12, psp\n"\
      "stmdb r12! , {r4-r11}\n"\
      "msr psp, r12\n");
  }

  toggle_led(0b100);
  current_time++;
  sysputc(',');
  sysputc('\n');
  sysputc('\r');

  if(!_first_time) {
    _next_task++;
    _next_task %= task_count;
  }

  // set PendSV and initiate context switch
  (*((int*)INTCTRL)) |= 1<<28;
  if (!_first_time) {
    asm volatile(
      "mrs r12, psp\n"
      "ldmfd r12! , {r4-r11}\n"
      "msr psp, r12\n"
      "isb");
  }
}

void c_entry(void) {
  __heap_top = _memtop;
  __heap_start = 0x20000500;

//  ClockInit();
  gpio_init();

  _memtop = __memtop - 0x100;
  _current_task = 0;
  _next_task = 0;
  task_count = 0;
  _first_time = 0;

  spawn_task(task1);
  spawn_task(task2);

  current_time = 0;
  _first_time = 1;

  asm volatile(" mrs r0, PRIMASK\n"
               " cpsid i\n");

  *((int*)STRELOAD) = 1000000;
  *((int*)STCURRENT) = 100;
  *((int*)NVIC) |= (1<<15) | (1<<14) | (1<<11) | (1<<1);
  *((int*)STCTRL) |= 0b011; //3;

  sysputc('W');

  asm volatile(" mrs r0, PRIMASK\n"
               " cpsie i\n");
}
