nextp8-bsp
==========

This is the Board Support Package (BSP) for the nextp8 core.

# Usage

When linking add `-L/path/to/nextp8-bsp -T/path/to/nextp8-bsp/nextp8-ram.ld` to
the command line, e.g.

```
m68k-elf-gcc ... -L/path/to/nextp8-bsp -T/path/to/nextp8-bsp/nextp8-ram.ld
```
