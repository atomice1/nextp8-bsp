# Copyright (C) 2025 Chris January
# Copyright (c) 1995, 1996, 2001 Cygnus Support
#
# The authors hereby grant permission to use, copy, modify, distribute,
# and license this software and its documentation for any purpose, provided
# that existing copyright notices are retained in all copies and that this
# notice is included verbatim in any distributions. No written agreement,
# license, or royalty fee is required for any of the authorized uses.
# Modifications to this software may be copyrighted by their authors
# and need not follow the licensing terms described here, provided that
# the new terms are clearly indicated on the first page of each file where
# they apply.   

SRCDIR = $(dir $(lastword $(MAKEFILE_LIST)))

TOOLCHAIN ?= m68k-elf-
CC := $(TOOLCHAIN)gcc
RANLIB := ranlib

ROM ?= 0
VERSION ?= 0

CFLAGS = -g -DVERSION=$(VERSION)

ifeq ($(ROM),1)
CFLAGS += -Os -DROM=1
NEXTP8_BSP = libnextp8-rom.a
CRT0 = crt0-rom.o
else
CFLAGS += -O2
NEXTP8_BSP = libnextp8-ram.a
CRT0 = crt0-ram.o
endif

OBJS=	isv.o crt1.o getpid.o kill.o sbrk.o \
		times.o usleep.o halt.o io.o \
		frame_buffer.o font.o postcode.o \
		stdio.o fatfs.o disk.o isrs1.o \
		sdblockdevice.o spi.o arith64.o \
		kbd.o error.o warm_reset.o time.o \
		exit.o gettimeofday.o version.o \
		format_version.o uart.o restart.o \
		shutdown.o
ISRS=	other_interrupt access_error address_error \
	illegal_instruction divide_by_zero privilege_violation \
	trace unimplemented_opcode breakpoint_debug_interrupt \
	format_error spurious_interrupt fp_interrupt \
	unsupported_instruction trap_interrupt
ISR_OBJS=	$(patsubst %,%.o,${ISRS})
IO_OBJS=	io-access.o io-close.o io-closedir.o \
		io-fstat.o io-isatty.o io-lseek.o io-mkdir.o \
		io-open.o io-opendir.o io-read.o io-readdir.o \
		io-rename.o io-stat.o io-system.o \
		io-unlink.o io-write.o
FATFS_SPI_OBJS= ff16/source/ffsystem.o \
				ff16/source/ffunicode.o \
				ff16/source/ff.o
INCLUDES =	-I$(SRCDIR)/../include -I$(SRCDIR)/ff16/source

all: $(CRT0) $(NEXTP8_BSP)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(patsubst %,%.o,${ISRS}) : isrs.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ \
	  -DL_$(patsubst %.o,%,$@)
$(NEXTP8_BSP): $(OBJS) $(IO_OBJS) $(FATFS_SPI_OBJS) $(ISR_OBJS)
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
$(CRT0): crt0.S
	$(CC) $(CFLAGS) -c $< -o $@

.PHONE: clean
clean:
	rm -f *.a *.o
