
#include "malloc.h"

#define CHIPS_IMPL

#include "probe.h"

#include "chips/z80.h"
#include "chips/ay38910.h"
#include "chips/i8255.h"
#include "chips/mc6845.h"
#include "chips/am40010.h"
#include "chips/upd765.h"
#include "chips/kbd.h"
#include "chips/mem.h"
#include "chips/clk.h"
#include "chips/fdd.h"
#include "chips/fdd_cpc.h"
#include "systems/cpc.h"

unsigned char* pixel_buffer;

#define MAX_SAMPLES 4096
float sample_buffer[MAX_SAMPLES];
unsigned int sample_count = 0;
char* bios_array = 0;

typedef struct {
    uint8_t kbd_joymask;      /* current joystick-1 state from keyboard-joystick emulation */
    uint8_t joy_joymask;      /* current joystick-1 state from cpc_joystick() */
    kbd_t kbd;                  /* keyboard matrix state */
} CPCControlsState;

void audio_callback_fn(const float* samples, int num_samples, void* user_data) {
    if (sample_count + num_samples < MAX_SAMPLES) {
        memcpy(sample_buffer+sample_count, samples, num_samples*sizeof(float));
        sample_count += num_samples;
    }
}

void machine_hardreset(cpc_t* sys) {
    cpc_desc_t desc;
    memset(sys, 0, sizeof(cpc_t));
    memset(&desc, 0, sizeof(cpc_desc_t));
    desc.type = CPC_TYPE_6128; //CPC_TYPE_464;
    desc.joystick_type = CPC_JOYSTICK_NONE; // DIGITAL;
    desc.pixel_buffer_size = _AM40010_MAX_FB_SIZE; //cpc_max_display_size();
    desc.pixel_buffer = pixel_buffer = malloc(desc.pixel_buffer_size);
    desc.rom_464_os = &bios_array[0x0];
    desc.rom_464_os_size = 16384;
    desc.rom_464_basic = &bios_array[0x4000];
    desc.rom_464_basic_size = 16384;
    desc.rom_6128_os = &bios_array[0x8000];
    desc.rom_6128_os_size = 16384;
    desc.rom_6128_basic = &bios_array[0xc000];
    desc.rom_6128_basic_size = 16384;
    desc.rom_6128_amsdos = &bios_array[0x10000];
    desc.rom_6128_amsdos_size = 16384;
    desc.rom_kcc_os = &bios_array[0x14000];
    desc.rom_kcc_os_size = 16384;
    desc.rom_kcc_basic = &bios_array[0x18000];
    desc.rom_kcc_basic_size = 16384;
    desc.audio_cb = audio_callback_fn;
    desc.audio_num_samples = CPC_MAX_AUDIO_SAMPLES;
    cpc_init(sys, &desc);
    //sys->pixel_buffer = desc.pixel_buffer;
}

cpc_t* machine_init(char* bios) {
    cpc_t* sys = (cpc_t*) malloc(sizeof(cpc_t));
    bios_array = bios;
    machine_hardreset(sys);
    return sys;
}

void machine_reset(cpc_t* sys) {
    cpc_reset(sys);
}

void machine_start_frame(cpc_t* sys) {
    kbd_update(&sys->kbd, 1000000/50);
}

void machine_tick(cpc_t* sys) {
    //cpc_tick(sys);
    cpc_exec(sys, 1); // TODO?
}

void machine_exec(cpc_t* sys, uint32_t micro_seconds) {
    cpc_exec(sys, micro_seconds);
}

void* machine_get_pixel_buffer(const cpc_t* sys) {
    return pixel_buffer;
}

float* machine_get_sample_buffer() {
    return sample_buffer;
}

unsigned int machine_get_sample_count() {
    unsigned int n = sample_count;
    sample_count = 0;
    return n;
}

uint8_t machine_mem_read(cpc_t* sys, uint16_t address) {
    return mem_rd(&sys->mem, address);
}
 
void machine_mem_write(cpc_t* sys, uint16_t address, uint8_t value) {
    mem_wr(&sys->mem, address, value);
}

int machine_get_state_size() {
    return sizeof(cpc_t);
}

int machine_get_cpu_state_size() {
    return sizeof(z80_t);
}

int machine_get_controls_state_size() {
    return sizeof(CPCControlsState);
}

void machine_save_state(const cpc_t* sys, cpc_t* state) {
    memcpy(state, sys, sizeof(cpc_t));
}

void machine_load_state(cpc_t* sys, const cpc_t* state) {
    memcpy(sys, state, sizeof(cpc_t));
}

void machine_save_cpu_state(const cpc_t* sys, z80_t* state) {
    memcpy(state, &sys->cpu, sizeof(z80_t));
}

void machine_load_cpu_state(cpc_t* sys, const z80_t* state) {
    memcpy(&sys->cpu, state, sizeof(z80_t));
}

void machine_save_controls_state(const cpc_t* sys, CPCControlsState* state) {
    state->kbd_joymask = sys->kbd_joymask;
    state->joy_joymask = sys->joy_joymask;
    state->kbd = sys->kbd;
}

void machine_load_controls_state(cpc_t* sys, const CPCControlsState* state) {
    sys->kbd_joymask = state->kbd_joymask;
    sys->joy_joymask = state->joy_joymask;
    sys->kbd = state->kbd;
}

void machine_key_down(cpc_t* sys, int key_code) {
    cpc_key_down(sys, key_code);
}

void machine_key_up(cpc_t* sys, int key_code) {
    cpc_key_up(sys, key_code);
}

bool machine_load_rom(cpc_t* sys, const uint8_t* ptr, int num_bytes) {
    return cpc_quickload(sys, ptr, num_bytes);
}

unsigned int machine_cpu_get_pc(cpc_t* sys) {
    return z80_pc(&sys->cpu);
}

unsigned int machine_cpu_get_sp(cpc_t* sys) {
    return z80_sp(&sys->cpu);
}

bool machine_cpu_is_stable(cpc_t* sys) {
    return z80_opdone(&sys->cpu);
}

int machine_get_raster_line(cpc_t* sys) {
    return sys->ga.crt.v_pos;
}

