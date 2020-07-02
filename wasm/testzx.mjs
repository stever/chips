
import * as zx from './zx.wasm';
console.log(zx)
zx.memory.grow(32);

import * as fs from 'fs';
const bios = fs.readFileSync('wasm/zxgeneric.rom')

const cArrayPointer = zx.malloc(0x5000);
console.log(cArrayPointer)

const cArray = new Uint8Array(
  zx.memory.buffer,
  cArrayPointer,
  0x5000
);
cArray.set(bios);

console.log('init')
const sys = zx.machine_init(cArrayPointer);
console.log(sys)

zx.machine_reset(sys)

console.log(zx.zx_std_display_width())
console.log(zx.zx_std_display_height())
console.log(zx.zx_max_display_size());

console.log(zx.zx_display_width(sys));
console.log(zx.zx_display_height(sys));

console.log('pixels', zx.machine_get_pixel_buffer(sys));

zx.machine_tick(sys);
zx.zx_exec(sys, 100000);

const pixels = new Uint8Array(
  zx.memory.buffer,
  zx.machine_get_pixel_buffer(sys),
  zx.zx_max_display_size()
);

fs.writeFileSync('testzx.rgba', pixels);

