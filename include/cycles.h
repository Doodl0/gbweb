#include "SDL3/SDL_stdinc.h"

extern Uint32 unwaitedCycles;

void Cycle_Add(Uint8 amount);
void Cycle_Wait();
Uint32 Cycle_Get();
void Cycle_Start();
