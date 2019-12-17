
#include "malloc.h"

#define CHIPS_IMPL

#include "chips/m6502.h"
#include "chips/m6526.h"
#include "chips/m6569.h"
#include "chips/m6581.h"
#include "chips/beeper.h"
#include "chips/kbd.h"
#include "chips/mem.h"
#include "chips/clk.h"
#include "systems/c64.h"

void audio_callback_fn(const float* samples, int num_samples, void* user_data) {
}

c64_t* c64_machine_init(char* bios) {
    c64_desc_t desc;
    c64_t* sys = (c64_t*) malloc(sizeof(c64_t));
    memset(sys, 0, sizeof(c64_t));
    memset(&desc, 0, sizeof(c64_desc_t));
    desc.pixel_buffer_size = c64_max_display_size();
    desc.pixel_buffer = malloc(desc.pixel_buffer_size);
    desc.rom_basic = &bios[0x0];
    desc.rom_char = &bios[0x2000];
    desc.rom_kernal = &bios[0x3000];
    desc.rom_basic_size = 0x2000;
    desc.rom_char_size = 0x1000;
    desc.rom_kernal_size = 0x2000;
    desc.audio_cb = audio_callback_fn;
    c64_init(sys, &desc);
    sys->pixel_buffer = desc.pixel_buffer;
    return sys;
}

void* c64_get_pixel_buffer(const c64_t* sys) {
    return sys->pixel_buffer;
}

float* c64_get_sample_buffer(const c64_t* sys) {
    return sys->sample_buffer;
}

