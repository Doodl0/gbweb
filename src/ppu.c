#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#define VRAM_BEGIN 0x8000
#define VRAM_END 0x9FFF

#define TILE_VALUE_ZERO
#define TILE_VALUE_ONE
#define TILE_VALUE_TWO
#define TILE_VALUE_THREE

const unsigned short VRAMSize = (VRAM_END - VRAM_BEGIN + 1);

// Set a constant frametime for an FPS limit
const Uint64 FPS = (1 / 59.7) * 1000000000;

Uint64 renderStart = 0;

typedef char tile[8][8];

struct VRAM{
    tile tileSet[384];
};

void RenderTimerInit() {
    renderStart = SDL_GetTicksNS();
}

int Render() {
    if (SDL_GetTicksNS() - renderStart < FPS) return 0;
    else {
        renderStart = SDL_GetTicksNS();
        return 1;
    }
}
