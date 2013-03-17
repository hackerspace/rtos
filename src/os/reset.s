#include<system.h>

.cpu cortex-m3
.syntax unified
.thumb

.word   0x20008000  /* stack top address */
.word   _start      /* 1 Reset */
.word   hang2       /* 2 NMI */
.word   hang3       /* 3 HardFault */
.word   hang4       /* 4 MemManage */
.word   hang5       /* 5 BusFault */
.word   hang6       /* 6 UsageFault */
.word   hang        /* 7 RESERVED */
.word   hang        /* 8 RESERVED */
.word   hang        /* 9 RESERVED*/
.word   hang        /* 10 RESERVED */
.word   _svcall     /* 11 SVCall */
.word   hang7       /* 12 Debug Monitor */
.word   hang        /* 13 RESERVED */
.word   _ctx_switcher        /* 14 PendSV */
.word   _systick        /* 15 SysTick */
.word   hang        /* 16 External Interrupt(0) */
.word   hang        /* 17 External Interrupt(1) */
.word   hang        /* 18 External Interrupt(2) */
.word   hang        /* 19 ...   */

.global __memtop
__memtop:         .word   0x20008000
.global __psp_stack_size
__psp_stack_size: .word   0x100
.global __msp_stack_size
__msp_stack_size: .word   0x100

.thumb_func
.global _start
_start:

    ldr r1, __memtop      /* Top of memory */

/*    ldr r2, __msp_stack_size */
    sbc r1, r1, #0x100

    msr psp, r1
    /* And finally subtract Kernel stack size to get final __memtop */
/*    ldr r2, __psp_stack_size */
    sbc r1, r1, #0x100
    ldr r0, __memtop
    msr msp, r0

/*    ldr r0, .Lbss_start
    mov r1, #0
    ldr r2, __memtop
    sub r2, r2, r0
    bl memset*/

/*    mov r0, sp
    push {r0}*/
    bl c_entry
/*    pop {r0}
    mov sp, r0*/
/*    ldr r0, __memtop*/
/*    msr psp, r0*/

/*    mov r1, #0
    msr    control, r1      /* Change the mode */
/*    isb*/

    bl sys_sleep
    b .
.Lc_entry:    .word c_entry
.Lbss_start:    .word __bss_start__
.Lbss_end:    .word __bss_end__

faultstat: .word 0xe000ed28

.thumb_func
hang2:
  mov r0, #0xfffffffd
  bx r0
  b hang
hang3:
  MRS     R0,PSP                  /* Read PSP */
  LDR     R1,[R0,#24]             /* Read Saved PC from Stack */
  BKPT #0
  mov r0, #0xfffffffd
  bx r0
  b hang
hang4:
  b hang
hang5:
  b hang
hang6:
  ldr r1, =faultstat
  ldr r1, [r1]
  b hang
hang7:
  b hang
hang8:
  b hang
hang:

  mov r0, #104
  bl sysputc
  wfi
  b hang
