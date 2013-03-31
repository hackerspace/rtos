#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include <stdint.h>
#define _SRAM_TOP_ 0x20004000


struct stack_frame {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t psr;
};

struct task {
  struct stack_frame *sp;
  int flags;
  int val;
};

extern struct task tasks[];
extern int _current_task;
extern int _next_task;
extern int _first_time;

void toggle_led(int id);

void c_entry(void);
void systick(void);
void context_sw(void);
//void syscall(int);
void syscall(int* svc_ins, struct stack_frame *psp);

int syscall_count();
extern int uart_b_tx;
extern int uart_b_rx;

#endif

