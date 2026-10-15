#include "SDL3/SDL_stdinc.h"

extern Uint32 tcycles;

void Cycles_AddM(Uint8 amount);
void Cycles_AddT(Uint8 amount);
void Cycles_Step();
Uint32 Cycles_Get();
