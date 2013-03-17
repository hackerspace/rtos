### Usage:

### QEMU
You need:
* arm-none-eabi toolchain, support for cortex-m3/4 (for example summon-arm-toolchain)
* qemu-system-arm from your distribution, now we have QEMU 1.2.2

You don't need:
* real HW :)

Running:
* make gdb

### Stellaris Launchpad - TI LM4F120XL ###

You need:
* arm-none-eabi toolchain, support for cortex-m3/4 (for example summon-arm-toolchain)
* lmicdi from lm4tools - https://github.com/utzig/lm4tools.git
  this is your gdbserver!
* real HW

Running:
* run lmicdi as your favourite superuser
* make clean && make && arm-none-eabi-gdb -q -nx -x "./gdbinit2" --tui ./bin/kernel.elf

type *load* into gdb, depending on linker settings your program should load into SRAM/FLASH - lambdapi.ld
type *continue* or *c* to run it

### Incompatibilities!

Now you need to recomment som parts when running in QEMU.

QEMU:
you can't access GPIO and some other periferies as well,
other periferals are typically on different address (UARTi).
So you can't toggle_led/1 for example.

comment out write in toggle_led in gpio.c + turn off clock init

STM32/LM4F:
devices are mapped to different address in mem. there are other init steps.
