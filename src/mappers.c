#include <SDL3/SDL_stdinc.h>
#include <memory.h>
typedef enum memoryBankControllers {
    MBC_NONE,
    MBC1,
    MBC1RAM,
    MBC1RAMBATTERY,
    MBC2,
    MBC2BATTERY,
    MBC3,
    MBC3RAM,
    MBC3RAMBATTERY,
} memoryBankControllers;

memoryBankControllers Mappers_GetMBC() {
    switch (Memory_ReadROM(0x147)) {
        case 0x00: return MBC_NONE;
        case 0x01: return MBC1;
        case 0x02: return MBC1RAM;
        case 0x03: return MBC1RAMBATTERY;
        case 0x05: return MBC2;
        case 0x06: return MBC2BATTERY;
        case 0x11: return MBC3;
        case 0x12: return MBC3RAM;
        case 0x13: return MBC3RAMBATTERY;
    }
}
