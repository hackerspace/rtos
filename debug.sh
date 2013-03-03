#!/bin/bash
make || exit

# Start up qemu in the background
qemu-system-arm -nographic -M lm3s811evb -cpu cortex-m3 -m 1 -serial telnet:127.0.0.1:1235,server,nowait -kernel ./bin/kernel.elf -s -S &

# And fire up the debugger
arm-none-eabi-gdb -nx -x "./gdbinit" --tui ./bin/kernel.elf
#arm-none-eabi-gdb -nx ./bin/kernel.elf 
