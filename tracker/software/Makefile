TOPDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
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
OBJS += src/systick.o
OBJS += ${OPENFATDIR}stm32/mmc.o

#************* Libopenfat
OPENFATDIR = src/openfat/
LIBOPENFAT = src/libopenfat.a

#************* CFlags
CFLAGS += -Wall -g -O0 -std=c99
CFLAGS += -DSTM32F0
CFLAGS += -I${OPENFATDIR}include
CFLAGS += -I${OPENFATDIR}stm32/f0/ 

#************* Linker Flags
LDSCRIPT = stm32f0-discovery.ld

LDFLAGS = -T${LDSCRIPT}
LDFLAGS += --static -nostartfiles
LDFLAGS += -Wl,-Map=$(*).map
LDFLAGS += -I/usr/arm-none-eabi/include
LDFLAGS += -L/usr/arm-none-eabi/lib
LDFLAGS += -I${OPENFATDIR}include


LDLIBS = -lopencm3_stm32f0
LDLIBS += -L${OPENFATDIR}/src/
LDLIBS += -lopenfat

#************

ARCHFLAGS = -mthumb -mcpu=cortex-m0 -msoft-float

.PHONY: clean

all: elf bin flash


elf: ${BINARY}.elf

bin: ${BINARY}.bin


flash: bin
	echo "reset halt; flash write_image erase unlock $(TOPDIR)/src/tracker.bin 0x8000000; reset" | nc 127.0.0.1 4444

clean:
	rm -rf *.o
	rm -rf ${BINARY}.elf
	rm -rf ${BINARY}.bin
	rm -rf ${BINARY}.map
	make -C src/openfat/src clean

${OPENFATDIR}${LIBOPENFAT}:
	make -C src/openfat/src libopenfat.a


%.o : %.c
	${CC} ${CFLAGS} ${ARCHFLAGS} -o $(*).o -c $(*).c

%.elf %.map: ${OBJS} ${OPENFATDIR}${LIBOPENFAT}
	${LD} ${LDFLAGS} ${ARCHFLAGS} ${OBJS} ${LDLIBS} -o $(*).elf

%.bin: %.elf
	${OBJCOPY} -Obinary $(*).elf $(*).bin


