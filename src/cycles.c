#include <SDL3/SDL_stdinc.h>
#include <cycles.h>
#include <SDL3/SDL_timer.h>

unsigned int unwaitedCycles = 0;

Uint64 cycleStart = 0;

// GB should run at 4.194304 MHz
const Uint64 CYCLE_TIME_NS = ((1 / (4.194304 * 1000000)) * 1000000000);

// Add the amount of cycles an instruction takes
void Cycle_Add(int amount) {
    unwaitedCycles += amount;
}

// Wait for the amount of time depending on instructions
void Cycle_Wait() {
    // Calculate time between start of instruction and current
    Uint64 timeElapsed = SDL_GetTicksNS() - cycleStart;
    // Amount of time the instruction should take
    Uint64 cycleTime = (unwaitedCycles * CYCLE_TIME_NS);

    // Prevent integer underflow and 5 century wait time
    if (timeElapsed > cycleTime) return;

    // Subtract the remaining time from the already elapsed time and delay
    SDL_DelayNS(cycleTime - timeElapsed);
}

unsigned int Cycle_Get() {
    return unwaitedCycles;
}

// Reset cycles and set timer start to current time
void Cycle_Start() {
    unwaitedCycles = 0;
    cycleStart = SDL_GetTicksNS();
}
