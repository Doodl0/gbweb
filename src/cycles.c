#include "SDL3/SDL_stdinc.h"
#include <cycles.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

unsigned int ticks = 0;

unsigned int unwaitedCycles = 0;

Uint64 cycleStart = 0;

// GB should run at 4.194304 MHz
const Uint64 CYCLE_TIME_NS = ((1 / (4.194304 * 1000000)) * 1000000000);

void Tick() {
    ticks++;
}

void Cycle_Add(int amount) {
    unwaitedCycles += amount;
}

void Cycle_Wait() {
    Uint64 timeElapsed = SDL_GetTicksNS() - cycleStart;
    Uint64 cycleTime = (unwaitedCycles * CYCLE_TIME_NS);
    // Prevent integer underflow and 5 century wait time
    if (timeElapsed > cycleTime) return;
    Uint64 timeToWait = cycleTime - timeElapsed;
    SDL_Log("%" PRIu64 ", %u, %" PRIu64 "", timeToWait, unwaitedCycles, timeElapsed);
    SDL_DelayNS(timeToWait);
}

unsigned int Cycle_Get() {
    return unwaitedCycles;
}

void Cycle_Start() {
    unwaitedCycles = 0;
    cycleStart = SDL_GetTicksNS();
}
