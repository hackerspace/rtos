#include<c_entry.h>

.cpu cortex-m3
.syntax unified
.thumb

/*.thumb_func
/*.global _enter
/*_enter:
/*    sub lr, lr, #4  /* Save adjusted LR_IRQ */
/*
/*  /*  cpsid i */
/*
/*    push  {r0-r12}  /* Save registers */
/*
/*    and r0, sp, #4  /* align the stack and save adjustment with LR_user */
/*    sub sp, sp, r0
/*    push  {r0, lr}
*/

_intctrl: .word 0xe000ed04

L_next_task: .word _next_task
L_current_task: .word _current_task
L_tasks: .word tasks
L_first: .word _first_time

.thumb_func
.global _svcall
_svcall:
  mrs r12, psp
  stmdb r12! , {r4-r11}
  msr psp, r12
  isb

  bl syscall

  mrs r12, psp
  ldmfd r12! , {r4-r11}
  msr psp, r12
  isb

  mov r0, #0xfffffffd
  bx r0


/*
  set PendSV

  ldr r2, _intctrl
  ldr r0, [r2]
  mov r1, #0
  movt r1, #1000
  orr r0, r1
  str r0, [r2]*/

.thumb_func
.global _systick
_systick:
  ldr r0, L_first
  ldr r0, [r0]
  cbz r0, __yes_save_it
  b __no_dont


__yes_save_it:

/*save context*/
  mrs r12, psp
  stmdb r12! , {r4-r11}
  msr psp, r12
  isb

__no_dont:
  bl systick

  ldr r0, L_first
  ldr r0, [r0]
  cbz r0, _yes_save_it
  b _no_dont

_yes_save_it:

  mrs r12, psp
  ldmfd r12! , {r4-r11}
  msr psp, r12
  isb

_no_dont:
  nop
/*  mov r0, #0xfffffffd
  bx r0*/



.thumb_func
.global _ctx_switcher
_ctx_switcher:
  ldr r0, L_first
  ldr r0, [r0]
  cbz r0, yes_save_it
  b no_dont


yes_save_it:

/*save context*/
  mrs r12, psp
  stmdb r12! , {r4-r11}
  msr psp, r12
  isb
/*  mov  r0, sp
   push {r0, lr}*/


/* we don't need to load MSR since we do only ASM now*/
  ldr r0, L_current_task
  ldr r0, [r0]
  lsl r0, #2
  ldr r1, L_tasks
  add r0, r1
  mrs r2, psp
  str r2, [r0]

/*  str sp, []*/

no_dont:
  bl lala

  ldr r0, L_next_task
  ldr r0, [r0]
  lsl r0, #2
  ldr r1, L_tasks
  add r0, r1
  ldr r2, [r0]

  msr psp, r2
  isb

.global __WAIT
__WAIT:
  mrs r12, psp
  ldmfd r12! , {r4-r11}
  ldr r3, [r12, #-28]
  msr psp, r12
  isb

#  ldr lr, [r2, #60]
  mov r0, #0xfffffffd
  bx r0

