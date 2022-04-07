#include "malloc.h"

#define CHIPS_IMPL

#include "probe.h"

#include "chips/z80.h"
#include "chips/beeper.h"
#include "chips/ay38910.h"
#include "chips/kbd.h"
#include "chips/mem.h"
#include "chips/clk.h"
#include "systems/zx.h"

#define MAX_SAMPLES 4096
float sample_buffer[MAX_SAMPLES];
unsigned int sample_count = 0;
char* bios_array = 0;

typedef struct {
    uint8_t kbd_joymask;      /* current joystick-1 state from keyboard-joystick emulation */
    uint8_t joy_joymask;      /* current joystick-1 state from zx_joystick() */
    kbd_t kbd;                  /* keyboard matrix state */
} ZXControlsState;

void audio_callback_fn(const float* samples, int num_samples, void* user_data) {
    if (sample_count + num_samples < MAX_SAMPLES) {
        memcpy(sample_buffer+sample_count, samples, num_samples*sizeof(float));
        sample_count += num_samples;
    }
}

void machine_hardreset(zx_t* sys) {
    zx_desc_t desc;
    memset(sys, 0, sizeof(zx_t));
    memset(&desc, 0, sizeof(zx_desc_t));
    desc.pixel_buffer_size = zx_max_display_size();
    desc.pixel_buffer = malloc(desc.pixel_buffer_size);
    desc.rom_zx48k = &bios_array[0x0];
    desc.rom_zx48k_size = 16384;
    desc.audio_cb = audio_callback_fn;
    desc.audio_num_samples = ZX_MAX_AUDIO_SAMPLES;
    zx_init(sys, &desc);
    sys->pixel_buffer = desc.pixel_buffer;
}

zx_t* machine_init(char* bios) {
    zx_t* sys = (zx_t*) malloc(sizeof(zx_t));
    bios_array = bios;
    machine_hardreset(sys);
    return sys;
}

void machine_reset(zx_t* sys) {
    zx_reset(sys);
}

void machine_start_frame(zx_t* sys) {
    kbd_update(&sys->kbd, 1000000/50);
}

void machine_tick(zx_t* sys) {
    //zx_tick(sys);
    zx_exec(sys, 1); // TODO?
}

void machine_exec(zx_t* sys, uint32_t micro_seconds) {
    zx_exec(sys, micro_seconds);
}

void* machine_get_pixel_buffer(const zx_t* sys) {
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

uint8_t machine_mem_read(zx_t* sys, uint16_t address) {
    return mem_rd(&sys->mem, address);
}
 
void machine_mem_write(zx_t* sys, uint16_t address, uint8_t value) {
    mem_wr(&sys->mem, address, value);
}

int machine_get_state_size() {
    return sizeof(zx_t);
}

int machine_get_cpu_state_size() {
    return sizeof(z80_t);
}

int machine_get_controls_state_size() {
    return sizeof(ZXControlsState);
}

void machine_save_state(const zx_t* sys, zx_t* state) {
    memcpy(state, sys, sizeof(zx_t));
}

void machine_load_state(zx_t* sys, const zx_t* state) {
    memcpy(sys, state, sizeof(zx_t));
}

void machine_save_cpu_state(const zx_t* sys, z80_t* state) {
    memcpy(state, &sys->cpu, sizeof(z80_t));
}

void machine_load_cpu_state(zx_t* sys, const z80_t* state) {
    memcpy(&sys->cpu, state, sizeof(z80_t));
}

void machine_save_controls_state(const zx_t* sys, ZXControlsState* state) {
    state->kbd_joymask = sys->kbd_joymask;
    state->joy_joymask = sys->joy_joymask;
    state->kbd = sys->kbd;
}

void machine_load_controls_state(zx_t* sys, const ZXControlsState* state) {
    sys->kbd_joymask = state->kbd_joymask;
    sys->joy_joymask = state->joy_joymask;
    sys->kbd = state->kbd;
}

void machine_key_down(zx_t* sys, int key_code) {
    zx_key_down(sys, key_code);
}

void machine_key_up(zx_t* sys, int key_code) {
    zx_key_up(sys, key_code);
}

void machine_load_rom(zx_t* sys, const uint8_t* ptr, int num_bytes) {
    // all 48K, start at $5ccb-$ff57
    if (num_bytes == 41613) {
        memcpy(&sys->ram[0][0x5ccb-0x4000], ptr, 41613);
        z80_set_pc(&sys->cpu, 0x5ccb);
        z80_set_sp(&sys->cpu, 0xff58);
    } else {
        zx_quickload(sys, ptr, num_bytes);
    }
}

unsigned int machine_cpu_get_pc(zx_t* sys) {
    return z80_pc(&sys->cpu);
}

unsigned int machine_cpu_get_sp(zx_t* sys) {
    return z80_sp(&sys->cpu);
}

bool machine_cpu_is_stable(zx_t* sys) {
    return z80_opdone(&sys->cpu);
}
