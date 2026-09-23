#define VRAM_BEGIN 0x8000
#define VRAM_END 0x9FFF

#define TILE_VALUE_ZERO
#define TILE_VALUE_ONE
#define TILE_VALUE_TWO
#define TILE_VALUE_THREE

const unsigned short VRAMSize = (VRAM_END - VRAM_BEGIN + 1);

typedef char tile[8][8];

struct GPU{
    tile tileSet[384];
};
