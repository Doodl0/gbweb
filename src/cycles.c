#include <SDL3/SDL_stdinc.h>
#include <cycles.h>
#include <cpu.h>
#include <ppu.h>

#define CYCLES_PER_FRAME 70224

Uint32 unwaitedCycles = 0;

Uint64 cycleStart = 0;

// GB should run at 4.194304 MHz
const Uint64 CYCLE_TIME_NS = ((1 / (4.194304 * 1000000)) * 1000000000);

// Add the amount of cycles an instruction takes (Multiply by 4 to get T cycles rather than m cycles)
void Cycles_Add(Uint8 amount) {
    unwaitedCycles += amount * 4;
}

void Cycles_Step() {
    while((unwaitedCycles < CYCLES_PER_FRAME) && !CPU_Halted()) {
        CPU_Step();
        PPU_Step();
    }
    unwaitedCycles -= CYCLES_PER_FRAME;
}
