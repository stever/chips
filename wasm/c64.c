
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

#define MAX_SAMPLES 4096
float sample_buffer[MAX_SAMPLES];
unsigned int sample_count = 0;

void audio_callback_fn(const float* samples, int num_samples, void* user_data) {
    if (sample_count + num_samples < MAX_SAMPLES) {
        memcpy(sample_buffer+sample_count, samples, num_samples*sizeof(float));
        sample_count += num_samples;
    }
}

c64_t* machine_init(char* bios) {
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

void machine_reset(c64_t* sys) {
    c64_reset(sys);
}

void machine_tick(c64_t* sys) {
    c64_tick(sys);
}

void machine_exec(c64_t* sys, uint32_t micro_seconds) {
    c64_exec(sys, micro_seconds);
}

void* machine_get_pixel_buffer(const c64_t* sys) {
    return sys->pixel_buffer;
}

float* machine_get_sample_buffer() {
    return sample_buffer;
}

unsigned int machine_get_sample_count() {
    unsigned int n = sample_count;
    sample_count = 0;
    return n;
}

uint8_t machine_mem_read(c64_t* sys, uint16_t address) {
    return mem_rd(&sys->mem_cpu, address);
}
 
void machine_mem_write(c64_t* sys, uint16_t address, uint8_t value) {
    mem_wr(&sys->mem_cpu, address, value);
}

int machine_get_state_size() {
    return sizeof(c64_t);
}

void machine_save_state(const c64_t* sys, void* state) {
    memcpy(state, sys, sizeof(c64_t));
}

void machine_load_state(c64_t* sys, const void* state) {
    memcpy(sys, state, sizeof(c64_t));
}

void machine_key_down(c64_t* sys, int key_code) {
    c64_key_down(sys, key_code);
}

void machine_key_up(c64_t* sys, int key_code) {
    c64_key_up(sys, key_code);
}

void machine_load_rom(c64_t* sys, const uint8_t* ptr, int num_bytes) {
    c64_quickload(sys, ptr, num_bytes);
}

unsigned int machine_cpu_get_pc(c64_t* sys) {
    return sys->cpu.PC;
}

unsigned int machine_cpu_get_sp(c64_t* sys) {
    return sys->cpu.S;
}

bool machine_cpu_is_stable(c64_t* sys) {
    return sys->cpu.PINS & M6502_SYNC;
}
