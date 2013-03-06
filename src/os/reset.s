#include<c_entry.h>

.cpu cortex-m3
.syntax unified
.thumb

.word   0x20002000  /* stack top address */
.word   _start      /* 1 Reset */
.word   hang        /* 2 NMI */
.word   hang        /* 3 HardFault */
.word   hang        /* 4 MemManage */
.word   hang        /* 5 BusFault */
.word   hang        /* 6 UsageFault */
.word   hang        /* 7 RESERVED */
.word   hang        /* 8 RESERVED */
.word   hang        /* 9 RESERVED*/
.word   hang        /* 10 RESERVED */
.word   _svcall     /* 11 SVCall */
.word   hang        /* 12 Debug Monitor */
.word   hang        /* 13 RESERVED */
.word   _ctx_switcher        /* 14 PendSV */
.word   systick        /* 15 SysTick */
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

    ldr r2, __psp_stack_size
    sbc r1, r1, r2

    msr psp, r1



    /* And finally subtract Kernel stack size to get final __memtop */
    ldr r2, __msp_stack_size
    sbc r1, r1, r2
    str r1, __memtop

    bl c_entry
    mov r1, #0
    msr    control, r1      /* Change the mode */
    isb

    bl sys_sleep
    b .
.Lc_entry:    .word c_entry

.thumb_func
hang:   b .
