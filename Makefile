
CLANG="$(EMSDK)/upstream/bin/clang"

test: wasm/c64.wasm wasm/zx.wasm wasm/vic20.wasm
	node --experimental-modules --experimental-wasm-modules wasm/testc64.mjs
	#convert -size 392x272 -depth 8 RGBA:testc64.rgba testc64.png
	node --experimental-modules --experimental-wasm-modules wasm/testzx.mjs
	node --experimental-modules --experimental-wasm-modules wasm/testvic20.mjs

# from https://dassur.ma/things/c-to-webassembly/
wasm/%.wasm: wasm/%.c systems/*.h chips/*.h wasm/probe.h
	$(CLANG) -v \
	-I. \
	-Iwasm \
	--target=wasm32 \
	-O2 \
	-flto \
	-nostdlib \
	-Wl,--no-entry \
	-Wl,--export-all \
	-Wl,--lto-O2 \
	-o $@ \
	$<

