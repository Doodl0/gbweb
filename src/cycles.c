#include <SDL3/SDL_stdinc.h>
#include <cycles.h>
#include <cpu.h>
#include <ppu.h>
#include <window.h>

#define CYCLES_PER_FRAME 70224

// Cycles occuring in this step
Uint32 tcycles = 0;

// Cycles occuring in the frametime
Uint32 tcyclesFrame = 0;


// GB should run at 4.194304 MHz
const Uint64 CYCLE_TIME_NS = ((1 / (4.194304 * 1000000)) * 1000000000);

// Add the amount of cycles an instruction takes (Multiply by 4 to get T cycles rather than m cycles)
void Cycles_AddM(Uint8 amount) {
    tcycles += amount * 4;
    tcyclesFrame += amount * 4;
}

void Cycles_AddT(Uint8 amount) {
    tcycles += amount;
    tcyclesFrame += amount;
}

Uint32 Cycles_Get() {
    return tcycles;
}

void Cycles_Reset() {
    tcycles = 0;
}

void Cycles_Step() {
    while((tcyclesFrame < CYCLES_PER_FRAME) && !CPU_Halted()) {
        CPU_Step();
        PPU_Step();
        Cycles_Reset();
    }

    tcyclesFrame -= CYCLES_PER_FRAME;
}
