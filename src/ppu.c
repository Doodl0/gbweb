#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <memory.h>
#include <ppu.h>
#include <main.h>

#define VRAM_BEGIN 0x8000
#define VRAM_END 0x9FFF

#define TILE_VALUE_ZERO
#define TILE_VALUE_ONE
#define TILE_VALUE_TWO
#define TILE_VALUE_THREE

extern SDL_Renderer *renderer;

typedef enum PPUMode {
    HBLANK,
    VBLANK,
    OAM,
    DRAW
} PPUMode;

const unsigned short VRAMSize = (VRAM_END - VRAM_BEGIN + 1);

typedef Uint8 tile[8][8];

Uint32 totalDotsForThisFrame = 0;

//SDL_Texture* framebuffer = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_TARGET, 160 * RESOLUTION_SCALE, 144 * RESOLUTION_SCALE);

void PPU_Render(Uint32 dots) {

}

void PPU_Step() {

}
