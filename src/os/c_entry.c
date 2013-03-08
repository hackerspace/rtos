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
int _memtop;
extern int __memtop;

int tasks[4] = {0,0,0,0};
int _current_task = 0;
int _next_task = 0;
int task_count = 0;
int _first_time = 0;

void syscall(int a) {
  sysputc(a);
}

void spawn_task(fp task) {
  const int sfrm_size = 8; //twice this
  tasks[task_count] = _memtop - sfrm_size*sizeof(int);
  int *s_frm = (int*)tasks[task_count];

  s_frm[0] = 0;
  s_frm[1] = 0;
  s_frm[2] = task_count+1;
  s_frm[3] = 0;
  s_frm[4] = 0;
  s_frm[5] = 0;
  s_frm[6] = (int)task-1;
  s_frm[7] = 0x21000000;
  tasks[task_count] -= sfrm_size*sizeof(int);

/*
  asm volatile (
      "push {r0-r2}\n"
      "ldr  r0, =_memtop\n"
      "ldr  r0, [r0]\n"
      "sub  r0, #24\n"
      "ldr  r1, =task_count\n"
      "ldr  r1, [r1]\n"
      "lsl r1, #2\n"
      "mov r2, %[task]\n"
      "add r0, r1\n" :: [task] "r" (task));
  asm volatile (
      "str r2, [r0]"
      );
  asm volatile (
      "pop {r0-r2}"
      );*/

  task_count++;
  _memtop -= 0x100;

  int bukakae = (int)task; //s_frm[1];
}

void task1() {
  short la = 0;
  while(1) {
    if (!la++)
    sysputc('1');
  }
}

void task2() {
  short la = 0;
  while(1) {
//    asm volatile("wfi");
    //asm volatile("svc #65");
    if (!la++)
    sysputc('2');
  }
}

int lala(void) {
  _current_task = _next_task;
  _first_time = 0;
  sysputc('C');
  return 5;
}

void systick(void) {
  if (!_first_time) {
  asm volatile (\
  "mrs r12, psp\n"\
  "stmdb r12! , {r4-r11}\n"\
  "msr psp, r12\n");
  }

  current_time++;
  sysputc(',');

  if(!_first_time) {
  _next_task++;
  _next_task %= task_count;
  }

  // set PendSV and initiate context switch
  (*((int*)INTCTRL)) |= 1<<28;
  if (!_first_time) {
  asm volatile("mrs r12, psp\n"
  "ldr r8, [r12, #8]\n"
  "ldmfd r12! , {r4-r11}\n"
  "msr psp, r12\n"
  "isb");
  }
}

void c_startup() {

}

void c_entry(void) {

  _memtop = __memtop - 0x100;

  spawn_task(task1);
  spawn_task(task2);

  current_time = 0;
  _first_time = 1;

  *((int*)STRELOAD) = 10000;
  *((int*)STCURRENT) = 0;
  *((int*)STCTRL) |= 3;
  *((int*)NVIC) |= (1<<15);

  //memcpy(0x0, tbl, sizeof(fp)*20);
  sysputc('W');

//  return tasks[0];

}
