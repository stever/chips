#!/bin/sh

~/emsdk/upstream/bin/clang -v \
   -I. \
   -Iwasm \
   --target=wasm32 \
   -O2 \
   -flto \
   -nostdlib \
   -Wl,--no-entry \
   -Wl,--export-all \
   -Wl,--lto-O2 \
   -o wasm/c64.wasm \
   wasm/c64.c

node --experimental-modules --experimental-wasm-modules wasm/test.mjs
convert -size 392x272 -depth 8 RGBA:test.rgba test.png

