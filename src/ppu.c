#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <memory.h>
#include <ppu.h>
#include <main.h>
#include <cycles.h>
#include <window.h>

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

static PPUMode mode = OAM;

// Dots are just the amount of T-cycles that the PPU need to run through
static Uint32 dots = 0;

// Current line to be rendered
Uint8 line = 0;

Uint8 PPU_Line() {
    return line;
}

// References for graphics: https://imrannazar.com/series/gameboy-emulation-in-javascript/graphics

#define LCD_CONTROL_BACKGROUND (1)
#define LCD_CONTROL_SPRITES (1 << 1)
#define LCD_CONTROL_SPRITESSIZE (1 << 2)
#define LCD_CONTROL_BGTILEMAP (1 << 3)
#define LCD_CONTROL_BGTILESET (1 << 4)
#define LCD_CONTROL_WINDOW (1 << 5)
#define LCD_CONTROL_WINDOWTILEMAP (1 << 6)
#define LCD_CONTROL_DISPLAY (1 << 7)

static Uint8 PPU_Register_LCD_Control(Uint8 bit) {
    return (Memory_Read(0xFF40) & bit) ? 1 : 0;
}

static Uint8 PPU_Register_ScrollX() {
    return Memory_Read(0xFF43);
}

static Uint8 PPU_Register_ScrollY() {
    return Memory_Read(0xFF42);
}

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
        sx = 1 << (7-x);

        // Update tileset
        tileset[tile][y][x] = ((Memory_ReadVRAM(address) & sx) ? 1 : 0) +
	       ((Memory_ReadVRAM(address + 1) & sx) ? 2 : 0);

    }
}

#define TILE_VALUE_ZERO {255,255,255,255}
#define TILE_VALUE_ONE {192,192,192,255}
#define TILE_VALUE_TWO {96,96,96,255}
#define TILE_VALUE_THREE {0,0,0,255}
// Get a 2 bit number and return the corresponding RGBA colour
pixelColour PPU_GetBGColour(Uint8 pixel) {
    switch (pixel) {
        case 0: return (pixelColour)TILE_VALUE_ZERO;
        case 1: return (pixelColour)TILE_VALUE_ONE;
        case 2: return (pixelColour)TILE_VALUE_TWO;
        case 3: return (pixelColour)TILE_VALUE_THREE;
        default: return (pixelColour)TILE_VALUE_ZERO;
    }
}

void PPU_DrawLine() {
    // Get scrollY register from memory
    Uint8 scy = PPU_Register_ScrollY();
    // Get scrollX register from memory
    Uint8 scx = PPU_Register_ScrollX();

    // VRAM offset for tilemap
    // Check memory for LCD control register and get background tilemap flag
    Uint16 mapOffset = PPU_Register_LCD_Control(LCD_CONTROL_BGTILEMAP) ? 0x1C00 : 0x1800;

    // Which lines of tiles to use in the map
    mapOffset += (((line + scy) & 255) >> 3) << 5;

    // Which tile to start with in the map line
    Uint16 lineOffset = (scx >> 3);

    // Which line of pixels to use in the tiles
    Uint16 y = (line + scy) & 7;

    // Where in the tileline to start
    Uint16 x = scx & 7;

    // Where to render on the canvas
    Uint16 canvasOffset = line * 160;

    // Read tile index from background map
    Uint16 tile = Memory_ReadVRAM(mapOffset + lineOffset);

    // Read tileset flag
    // If the tile data set in use is #1, the
	// indices are signed; calculate a real tile offset
    Uint8 bgtile = PPU_Register_LCD_Control(LCD_CONTROL_BGTILESET);
    if ((bgtile != 1) && (tile < 128)) tile += 256;

    for(Uint8 i = 0; i < 160; i++) {
        // Map tile data to colour through palette
        int tiledata = tileset[tile][y][x];
        pixelColour colour0 = PPU_GetBGColour(tiledata);

        // Draw pixel to framebuffer
        Window_DrawPixel(canvasOffset, colour0);
        canvasOffset += 1;
        // When tile ends, read another
        x++;
        if(x == 8) {
			x = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = Memory_ReadVRAM(mapOffset + lineOffset);
			if ((bgtile != 1) && (tile < 128)) tile += 256;
		}
    }
}

void PPU_Step() {
    // Reference : https://imrannazar.com/series/gameboy-emulation-in-javascript/gpu-timing
    dots += Cycles_Get();

    switch(mode) {
        // OAM read mode, scanline active
        case OAM:
            if (dots >= 80) {
                // Enter VRAM read / draw mode (mode 3)
                dots -= 80;
                mode = DRAW;
            }
            break;

        // VRAM read / draw mode, scanline active
        // Treat end of mode 3 as end of scanline
        case DRAW:
            if (dots >= 172) {
                // Enter HBLANK
                dots -= 172;
                mode = HBLANK;

                // Write a scanline to the framebuffer
                PPU_DrawLine();
            }
            break;

        // HBLANK
        // After last HBLANK, push framebuffer to screen
        case HBLANK:
            if (dots >= 204) {
                if (line == 143) {
                    // Enter VBLANK
                    mode = VBLANK;

                    // Push image to screen
                    Window_Blit();
                }
                else mode = OAM;
                line++;
                dots -= 204;
            }
            break;

        // VBLANK (10 lines)
        case VBLANK:
            if (dots >= 456) {
                line++;
                if (line == 153) {
                    // Restart scanning modes
                    mode = OAM;
                    line = 0;
                }

                dots -= 456;
            }
            break;
    }
}
