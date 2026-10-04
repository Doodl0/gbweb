#include <cycles.h>
#include <SDL3/SDL_log.h>

unsigned int ticks = 0;

unsigned int unwaitedCycles = 0;

void Tick() {
    ticks++;
}

void Cycle(int amount) {
    unwaitedCycles += amount;
}

unsigned int CyclesGet() {
    return unwaitedCycles;
}

void CyclesReset() {
    unwaitedCycles = 0;
}
