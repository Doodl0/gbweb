#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <memory.h>
#include <ppu.h>
#include <main.h>
#include <cycles.h>

#define TILE_VALUE_ZERO 255,255,255,255
#define TILE_VALUE_ONE 192,192,192,255
#define TILE_VALUE_TWO 96,96,96,255
#define TILE_VALUE_THREE 0,0,0,255

#define VRAM_BEGIN 0x8000
#define VRAM_END 0x9FFF

const Uint16 VRAM_SIZE = (VRAM_END - VRAM_BEGIN + 1);
#define TILES_AMOUNT 384

// Tileset
Uint8 tileset[TILES_AMOUNT][8][8];

typedef enum PPUMode {
    HBLANK,
    VBLANK,
    OAM,
    DRAW
} PPUMode;

static PPUMode mode = HBLANK;

// Dots are just the amount of T-cycles that the PPU need to run through
static Uint32 dots = 0;

// Current line to be rendered
static Uint8 line = 0;



//SDL_Texture* framebuffer = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_TARGET, 160 * RESOLUTION_SCALE, 144 * RESOLUTION_SCALE);

// Take values from
void PPU_UpdateTile(Uint16 address) {
    // Get base address for tile
    address &= 0x1FFE;

    // Find the row and tile that was updated
    Uint16 tile = (address >> 4) & 511;
    Uint16 y = (address >> 1) & 7;
    Uint8 sx;

    for (Uint8 x = 0; x < 8; x++) {
        // Find bit index for the pixel
        sx  = 1 << (7-x);

        // Update tileset
        tileset[tile][y][x] = ((Memory_ReadVRAM(address) & sx) ? 1 : 0) +
	        ((Memory_ReadVRAM(address + 1) & sx) ? 2 : 0);

    }
}

void PPU_DrawPixel() {

}

void PPU_DrawLine() {

}

void PPU_Step() {
    // Reference : https://imrannazar.com/series/gameboy-emulation-in-javascript/gpu-timing

    dots += Cycles_Get();

    switch(mode) {
        // OAM read mode, scanline active
        case OAM:
            if (dots >= 80) {
                // Enter VRAM read / draw mode (mode 3)
                dots = 0;
                mode = DRAW;
            }
            break;

        // VRAM read / draw mode, scanline active
        // Treat end of mode 3 as end of scanline
        case DRAW:
            if (dots >= 172) {
                // Enter HBLANK
                dots = 0;
                mode = HBLANK;

                // Write a scanline to the framebuffer
                PPU_DrawLine();
            }
            break;

        // HBLANK
        // After last HBLANK, push framebuffer to screen
        case HBLANK:
            if (dots >= 204) {
                dots = 0;
                line++;

                if (line == 143) {
                    // Enter VBLANK
                    mode = VBLANK;
                    // TODO: push image to screen
                }
                else mode = OAM;
            }
            break;

        // VBLANK (10 lines)
        case VBLANK:
            if (dots >= 456) {
                dots = 0;
                line++;

                if (line > 153) {
                    // Restart scanning modes
                    mode = OAM;
                    line = 0;
                }
            }
            break;
    }
}
