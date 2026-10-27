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

    // ROM has a max of 256 banks, which is (256 * 16KiB) which is 4MiB
    Uint8 ROM[0x400000];
    Uint8 cartridgeRAM[0x8000];
} memoryBus;

extern memoryBus memory;
extern Uint8 ime;

void Registers_WriteR8(r8Enum target, Uint8 value);

Uint8 Registers_ReadR8(r8Enum r);

void Memory_Write(Uint16 address, Uint8 value);

Uint8 Memory_Read(Uint16 address);

Uint8 Memory_ReadVRAM(Uint16 address);

Uint8 Memory_ReadROM(Uint16 address);

void Memory_SetROMBank(Uint8 sourceBank, Uint8 destinationBank);

void Memory_SetRAMBank(Uint8 sourceBank);

void Memory_Set(Uint8* buffer, Uint32 size);

void Memory_LogInit();

void Memory_Log(memoryBus memory, registersStruct registers);

void Memory_Deinit();
