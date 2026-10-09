#include <SDL3/SDL_stdinc.h>
#include <registers.h>

typedef struct memoryBus {
    union {
        struct {
            Uint8 ROMBank0[0x4000];
            Uint8 ROMBank1[0x4000];
            Uint8 VRAM[0x2000];
            Uint8 externalRAM[0x4000];
            Uint8 WRAM0[0x1000];
            Uint8 WRAM1[0x1000];
            Uint8 echoRAM[0x1E00];
            Uint8 OAM[0xA0];
            Uint8 prohibited[0x60];
            Uint8 IO[0x80];
            Uint8 HRAM[0x7F];
            Uint8 IE;
        };
        Uint8 memory[0x10000];
    };
} memoryBus;

extern memoryBus memory;
extern Uint8 ime;

void Memory_Write(Uint16 address, Uint8 value);

Uint16 Memory_Read(Uint16 address);

void Memory_Set(Uint8* buffer, Uint32 size);

void Memory_LogInit();

void Memory_Log(memoryBus memory, registersStruct registers);

void Memory_Deinit();
