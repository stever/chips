# chips

A toolbox of 8-bit chip-emulators, helper code and complete embeddable 
system emulators in dependency-free C headers (a subset of C99 that
compiles on gcc, clang and cl.exe).

Tests and example code is in a separate repo: https://github.com/floooh/chips-test

The example emulators, compiled to WebAssembly: https://floooh.github.io/tiny8bit/

For schematics, manuals and research material, see: https://github.com/floooh/emu-info

## WebAssembly support

@sehugg added WASM support for [8bitworkshop](https://8bitworkshop.com/).
You'll need to install [EMSDK](https://github.com/emscripten-core/emsdk)
first. Then type `make`.

## System Emulators

The directory ```systems``` contains a number of header-only 8-bit
computer emulators which can be embedded into applications that
provide keyboard input, and render the emulator's generated
video- and audio-output.

Note that accuracy of the system emulators varies quite a lot,
and is mainly defined by what games, demos and tests have been
used for testing and improving the emulation.

The following system emulators are provided:

### ZX Spectrum 48k and 128

The Sinclair ZX Spectrum is supported as the original 48k model and
the improved 128 model with a proper sound chip (the AY-3-8912, which was 
also used in the Amstrad CPC).

## Chip Emulators

### Z80 CPU (chips/z80.h)

The Zilog Z80 CPU.

- tick-callback with CPU-pin bitmask, called with machine-cycle or single-tick granularity
- fast switch-case instruction decoder generated via python script
- up to 7 wait states can be injected per machine cycle by setting WAIT pins in 
  the CPU tick callback
- all undocumented instructions supported
- internal WZ register and undocumented XF and YF flags supported
- support for interrupt-priority handling (daisy chain) with help from the tick callback
- runs the ZEXDOC and ZEXALL tests
- runs the CPU test of the FUSE ZX Spectrum emulator, with the following exceptions:
  - the state of the XF/YF undocumented flags is ignored for indirect BIT
  test instructions, FUSE doesn't agree here with ZEXALL and I think ZEXALL
  is right (the state of the XF/YF flags depends on the current state of the
  internal WZ register)
  - FUSE assumes that the PC after a HALT instruction has been incremented,
  while the chips Z80 emulator doesn't incrmenent the PC, this shouldn't make
  any difference though
- properly handles sequences of DD/FD prefix bytes
- flexible trap callback for hooking in debuggers and "native code" handlers
- NOT IMPLEMENTED/TODO:
    - interrupt mode 0
    - refresh cycle in second half of opcode fetch machine cycle
    - bus request/acknowledge (BUSRQ/BUSAK pins)

### AY-3-8910 (chips/ay38910.h)

The General Instrument AY-3-8910 sound generator chip and its low-cost variants
AY-3-8912 and AY-3-8913 (the 3 variants only differ in the number of provided
I/O ports, the 8910 has 2 ports, the 8912 has 1, and the 8913 has none.

- programmable via chip-pin bitmask
- separate tick function called from CPU tick callback, returns true when a new audio sample is ready

## Helper Code

### Memory (chips/mem.h)

Helper code for 8-bit home computer memory systems.

- map 16-bit address ranges to host memory with 1-KByte page granularity
- memory pages can be mapped as RAM, ROM or RAM-behind-ROM via separate read/write pointers
- up to 4 'mapping layers' to simplify bank-switching or memory expansion systems

### Keyboard Matrix (chips/kbd.h)

Helper code for keyboard matrix typically found in 8-bit home computers.

- map host system key- or ASCII-codes to keyboard matrix locations
- up to 12x12 matrix size
- up to 4 configurable modifier keys (for shift keys)
- internal pressed-key-buffer for simultaneously pressed host system keys
- configurable 'sticky count' to lengthen short host-system key presses to
give the emulated system enough time to scan the keyboard matrix state

### Sound Beeper (chips/beeper.h)

A square-wave beeper found in many simple home computers.

- toggle beeper state on/off from CPU tick callback
- tick function returns true when new audio-sample is ready

### Clock (chips/clk.h)

Helper function to convert a clock frequency in Hz to a number of ticks,
and to keep track of the 'left over' ticks from one frame to the next.
