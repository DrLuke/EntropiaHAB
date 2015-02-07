PREFIX		= arm-none-eabi

CC		:= $(PREFIX)-gcc
CXX		:= $(PREFIX)-g++
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
GDB		:= $(PREFIX)-gdb

BINARY = src/tracker

OBJS += ${BINARY}.o
OBJS += src/ax25.o
OBJS += src/dac.o
OBJS += src/aprs.o

#************* CFlags
CFLAGS += -Wall -g -O0 -std=c99

CFLAGS += -DSTM32F0

#************* Linker Flags
LDSCRIPT = stm32f0-discovery.ld

LDFLAGS = -T${LDSCRIPT}
LDFLAGS += --static -nostartfiles
LDFLAGS += -Wl,-Map=$(*).map
LDFLAGS += -I/usr/arm-none-eabi/include
LDFLAGS += -L/usr/arm-none-eabi/lib


LDLIBS = -lopencm3_stm32f0

#************

ARCHFLAGS = -mthumb -mcpu=cortex-m0 -msoft-float

.PHONY: clean

all: elf bin flash


elf: ${BINARY}.elf

bin: ${BINARY}.bin


flash:
	#Something with openocd here

clean:
	rm -rf *.o
	rm -rf ${BINARY}.elf
	rm -rf ${BINARY}.bin
	rm -rf ${BINARY}.map

%.o : %.c
	${CC} ${CFLAGS} ${ARCHFLAGS} -o $(*).o -c $(*).c

%.elf %.map: ${OBJS}
	${LD} ${LDFLAGS} ${ARCHFLAGS} ${OBJS} ${LDLIBS} -o $(*).elf

%.bin: %.elf
	${OBJCOPY} -Obinary $(*).elf $(*).bin


