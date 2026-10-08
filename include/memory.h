#include <SDL3/SDL_stdinc.h>
#include <registers.h>

typedef struct memoryBus {
    Uint8 memory[0xFFFF];
} memoryBus;

extern memoryBus memory;
extern Uint8 ime;

void Memory_Write(Uint16 address, Uint8 value);

Uint16 Memory_Read(Uint16 address);

void Memory_Set(Uint8* buffer, Uint32 size);

void Memory_LogInit();

void Memory_Log(memoryBus memory, registersStruct registers);

void Memory_LogDeinit();
