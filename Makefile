ARCH = arm-none-eabi
CC = ${ARCH}-gcc
CPP = ${ARCH}-g++
AS = ${ARCH}-as
LD = ${ARCH}-ld
AR = ${ARCH}-ar
OBJCOPY = ${ARCH}-objcopy

CPU =
TOOLS_VERSION = $(shell $(CC) -dumpversion)

ifndef PLATFORM
PLATFORM = qemu
endif

ifdef RELEASE
CFLAGS = -O0 -std=gnu99 -Werror -DPLATFORM=$(PLATFORM) -DRASPBERRY_PI
ASFLAGS =
else
CFLAGS = -O0 -g -std=gnu99 -Werror -DPLATFORM=$(PLATFORM) -DRASPBERRY_PI
#-DLM4F
ASFLAGS = -g
endif

CFLAGS_FOR_TARGET = -mcpu=cortex-m4 -mthumb -ggdb3 -mfloat-abi=soft #arm1176jzf-s
ASFLAGS_FOR_TARGET = -mcpu=cortex-m4 -mthumb -ggdb3 -mfloat-abi=soft #arm1176jzf-s
LDFLAGS = -nostdlib -static
#--error-unresolved-symbols

SYSLIBS = ~/sat/lib/gcc/arm-none-eabi/4.7.3/libgcc.a
#SYSLIBS = /usr/local/lib/gcc/$(ARCH)/$(TOOLS_VERSION)/libgcc.a
#SYSLIBS = ./newlib/usr/lib/gcc/$(ARCH)/$(TOOLS_VERSION)/libgcc.a
NEWLIB_DIR = newlib/usr/$(ARCH)
NEWLIB_LIBC = $(NEWLIB_DIR)/lib/thumb/$(CPU)/libc.a
NEWLIB_LIBM = $(NEWLIB_DIR)/lib/thumb/$(CPU)/libm.a

MODULES := os monitor
#bsp bsp/generic bsp/$(PLATFORM) lib lambda
SRC_DIR := $(addprefix src/,$(MODULES))
INC_DIR := $(addsuffix /include,$(SRC_DIR))

ASRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.s))
AOBJ     := $(ASRC:.s=.o)
CSRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
COBJ     := $(CSRC:.c=.o)

INCLUDES := -Isrc -Isrc/include $(addprefix -I,$(SRC_DIR) $(INC_DIR)) -I$(NEWLIB_DIR)/include

vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.s $(SRC_DIR)

%.o: %.c
	$(CC) $(CFLAGS_FOR_TARGET) $(INCLUDES) $(CFLAGS) -c -o $*.o $<

%.o: %.s
	$(AS) $(ASFLAGS_FOR_TARGET) $(INCLUDES) $(ASFLAGS) -o $*.o $<

OBJ = $(AOBJ) $(COBJ) $(CPPOBJ)

bin/kernel.img: bin/kernel.elf
	${OBJCOPY} -O binary $< $@

bin/kernel.elf: lambdapi.ld $(OBJ) $(SYSLIBS)
	${LD} ${LDFLAGS} -T lambdapi.ld $(OBJ) $(NEWLIB_LIBC) $(NEWLIB_LIBM) $(SYSLIBS) -o $@

install-newlib:
	newlib/build-newlib.sh

clean:
	rm -f bin/*.elf bin/*.img $(OBJ)

all: bin/kernel.img

gdb: bin/kernel.img
	killall qemu-system-arm || true
	# Start up qemu in the background
	terminal --default-working-directory=$(pwd) -e 'arm-none-eabi-gdb -q -nx -x "./gdbinit" --tui ./bin/kernel.elf' &
	qemu-system-arm -nographic -M lm3s811evb -cpu cortex-m3 -m 2 -serial telnet:127.0.0.1:1235,server,nowait -serial telnet:127.0.0.1:1236,server,nowait -kernel ./bin/kernel.elf -s -S
	# And fire up the debugger

dump: bin/kernel.elf
	arm-none-eabi-objdump -D bin/kernel.elf

help:
	@echo
	@echo make install-newlib
	@echo make clean
	@echo make [RELEASE=1] [PLATFORM='(raspi|qemu)']
	@echo
