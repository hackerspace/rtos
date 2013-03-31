#include <sleep.h>
#include <system.h>
#include <string.h>
#include <uart.h>
#include <clock.h>
#include <stdio.h>
#include <gpio.h>

#include <unistd.h>

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

struct task tasks[4];
int _current_task = 0;
int _next_task = 0;
int task_count = 0;
int _first_time = 0;

int __heap_top;
int __heap_start;

void syscall_(int a,int b,int c,int d) {
  asm volatile("swi #0");
}

void pche(int d) {
  syscall_(0, d,0,0);
}

int svc_val() {
  // TODO: this is VERY BAD
  // and will probably NOT work in presence of MPU
  return tasks[_current_task].val;
}

int kokotina(int a) {
  int r0 = 1, r1 = a, r2, r3;
  syscall_(r0, r1, r2, r3);

  return svc_val();
}

int syscall_count() {
  int r0 = 2, r1, r2, r3;
  syscall_(r0, r1, r2, r3);
  return svc_val();
}

void print_num(int num);
void syscall(int* svc_ins, struct stack_frame *psp) {
  static int count = 0;

  count++;

  int call_id = psp->r0;
  int arg0    = psp->r1;
  int arg1    = psp->r2;
  int arg2    = psp->r3;

  switch (call_id) {
    case 0:
      printf("ahoj ");print_num(arg0);fflush(stdout);
      printf("\n\r");
      break;
    case 1:
//      psp->r0 = (_current_task+2) * arg0;
      tasks[_current_task].val = (_current_task+2) * arg0;
      break;
    case 2:
      tasks[_current_task].val = count;
      break;
    case 0x48:
      toggle_led(arg0);
      break;
  }

  //sysputc(_current_task+'1');
}

void spawn_task(fp task) {
  struct stack_frame *s_frm =
    (struct stack_frame *)(_memtop - sizeof(struct stack_frame));

  s_frm->pc = (uint32_t)(task-1);
  //where to jump
  s_frm->psr = 0x21000000;
  //default program status word

  tasks[task_count].sp = s_frm;
  //save pointer to top of tasks stack
  tasks[task_count].sp -= 1;
  //move 'one stack' down  //sfrm_size*sizeof(int);
  tasks[task_count].flags = 0;

  task_count++;
  _memtop -= 0x100;
}

void idle_task() {
  write(2, "\ec",2);
  uart_setecho(1, 1);

  while (1) {
    char b[16];
//    scanf("%s\n", b);
    read(2, b, 16);
    write(2, "\r\n", 2);
    if (strncmp(b, "echo", 4) == 0) {
      write(2, b+5, 16-5);
      write(2, "\r\n", 2);
      pche(666);
      int la = kokotina(17);
      print_num(la);fflush(stdout);
      write(3, "\r\nhelon\r\n", 7);
    }
    //FILE *s1 = fopen("s1", "w");
//    fprintf(stderr, "helon\n");//fflush(stderr);
//    write(3, "helon\n", 6);
    asm volatile ("wfi");
  }
}

void task1() {
  int a;

//printf("task 1\n\r");

//while(1) {};

  while(1) {
    a=0;
    while(a++!=10000);
//    toggle_led(0b100);
  //  printf("hahah");
//    printf("!\n\r");
    sysputc('~');
    if (a%3 == 0) {
      sysputc('\n');
      sysputc('\r');
    }
  }
}

void task2() {
  int a = 0;
//printf("task 2\n\r");
//scanf("%d\n", &a);
//printf("you've entered %a, you bitch!\t\n");
  while(1) {
    a=0;
    while(a++!=10000);
//    toggle_led(0b001);
//    asm volatile("mov r0, #1\n svc #0");
    //printf("!\n\r");
    sysputc('!');
    if (a%3 == 0) {
      sysputc('\n');
      sysputc('\r');
    }
  }
}

void context_sw(void) {
  _current_task = _next_task;
  _first_time = 0;
//  toggle_led(0b100);
  //sysputc('C');
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
  //sysputc(',');
  //sysputc('\n');
  //sysputc('\r');

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

extern void monitor_main();
void c_entry(void) {
  __heap_top = _memtop;
  __heap_start = 0x20000500;

  ClockInit();
  gpio_init();
  uart_init();

  _memtop = __memtop - 0x100;
  _current_task = 0;
  _next_task = 0;
  task_count = 0;
  _first_time = 0;

  spawn_task(idle_task);
//  spawn_task(task1);
//  spawn_task(task2);
  spawn_task(monitor_main);

  current_time = 0;
  _first_time = 1;

  asm volatile(" mrs r0, PRIMASK\n"
               " cpsid i\n");

  //GPIO_PORTF_IM_R |= 1<<4;
  //GPIO_PORTF_ICR_R |= 0b10000;

  *((int*)STRELOAD) = 10000;
  *((int*)STCURRENT) = 100;
  *((int*)NVIC) |= (1<<22) | (1<<21) | (1<<16) | (1<<15) | (1<<14) | (1<<11) | (1<<1);
  //*(((int*)NVIC)+1) |= (1<<(46-32));
  *((int*)STCTRL) |= 0b011; //3;

//  NVIC_PEND0_R |= NVIC_PEND0_INT21;
  NVIC_EN0_R |= NVIC_EN0_INT5;
  NVIC_EN0_R |= NVIC_EN0_INT6;
  sysputc('W');

  asm volatile(" mrs r0, PRIMASK\n"
               " cpsie i\n");
  (*(int*)(0xe000ef00)) = 21;
}
