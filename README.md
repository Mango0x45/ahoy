# Ahoy! — The CHIP-8 Emulator and -Toolchain


Ahoy! (the ‘!’ is part of the name) is a CHIP-8 emulator and -toolchain.
As of writing, the following binaries exist as part of the Ahoy!
distribution:

- `ahoy` — GUI emulator
- `c8asm` — CHIP-8 assembler
- `c8dump` — CHIP-8 disassembler


## Where does the name come from?

The ‘Chips Ahoy!’ cookies I enjoyed as a child.


## Building and Installation

The entire project can be built with a C23 compiler:

```sh
$ cc -std=c23 -o make make.c
$ ./make          # compile everything
$ ./make install  # install everything
```

The following flags can be provided to the build script:

- `-f` / `--force`: force a complete rebuild
- `-r` / `--release`: build with optimizations enabled


## Documentation

All programs in the Ahoy! collection are documented via manual pages.
You can read them either with the `man` command, or by passing the
`-h`/`--help` flag to the given program.


## Potential Future Plans?

- TUI emulator via ncurses
- Compiler for a C-like higher-level language
- SCHIP support
- Handheld emulator on an embedded system
