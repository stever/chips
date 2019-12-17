
import * as c64 from './c64.wasm';
console.log(c64)
c64.memory.grow(32);

import * as fs from 'fs';
const bios = fs.readFileSync('wasm/c64generic.rom')

const cArrayPointer = c64.malloc(0x5000);
console.log(cArrayPointer)

const cArray = new Uint8Array(
  c64.memory.buffer,
  cArrayPointer,
  0x5000
);
cArray.set(bios);

console.log('init')
const sys = c64.machine_init(cArrayPointer);
console.log(sys)

c64.machine_reset(sys)

console.log(c64.c64_std_display_width())
console.log(c64.c64_std_display_height())
console.log(c64.c64_max_display_size());

console.log(c64.c64_display_width(sys));
console.log(c64.c64_display_height(sys));

console.log('pixels', c64.machine_get_pixel_buffer(sys));

c64.machine_tick(sys);
c64.c64_exec(sys, 100000);

const pixels = new Uint8Array(
  c64.memory.buffer,
  c64.machine_get_pixel_buffer(sys),
  c64.c64_max_display_size()
);

fs.writeFileSync('test.rgba', pixels);

