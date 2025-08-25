# Copyright (C) 2025 Chris January
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

all: lib-rom lib-ram

.PHONY: lib-ram
lib-ram:
	mkdir -p build/ram/ff16/source
	cd build/ram && $(MAKE) -f ../../src/Makefile VPATH=../../src
	mkdir -p lib
	cp -u build/ram/libnextp8-ram.a lib/
	cp -u build/ram/crt0-ram.o lib/

.PHONY: lib-rom
lib-rom:
	mkdir -p build/rom/ff16/source
	cd build/rom && $(MAKE) -f ../../src/Makefile VPATH=../../src ROM=1
	mkdir -p lib
	cp -u build/rom/libnextp8-rom.a lib/
	cp -u build/rom/crt0-rom.o lib/

.PHONY: clean
clean:
	cd build/ram && $(MAKE) -f ../../src/Makefile clean
	cd build/rom && $(MAKE) -f ../../src/Makefile clean
	rm -f lib/libnextp8.a
	rm -f lib/libnextp8-rom.a

