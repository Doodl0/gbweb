#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <memory.h>

#define VRAM_BEGIN 0x8000
#define VRAM_END 0x9FFF

#define TILE_VALUE_ZERO
#define TILE_VALUE_ONE
#define TILE_VALUE_TWO
#define TILE_VALUE_THREE

const unsigned short VRAMSize = (VRAM_END - VRAM_BEGIN + 1);

typedef Uint8 tile[8][8];

void Render() {

}
