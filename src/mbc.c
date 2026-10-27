#include <SDL3/SDL_stdinc.h>
#include <memory.h>

// Reference: https://tonisagrista.com/blog/2026/playkid/#cartridges

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

memoryBankControllers currentMBC = MBC_NONE;
bool RAMEnabled = false;
Uint8 romBank = 1;
Uint8 ramBank = 0;
Uint8 bankingMode = 0;

memoryBankControllers MBC_GetMBC() {
    switch (Memory_ReadROM(0x147)) {
        case 0x00: return MBC_NONE; break;
        case 0x01: return MBC1; break;
        case 0x02: return MBC1RAM; break;
        case 0x03: return MBC1RAMBATTERY; break;
        case 0x05: return MBC2; break;
        case 0x06: return MBC2BATTERY; break;
        case 0x11: return MBC3; break;
        case 0x12: return MBC3RAM; break;
        case 0x13: return MBC3RAMBATTERY; break;
    }
    return MBC_NONE;
}

void MBC_WriteMBC1(Uint16 address, Uint8 value) {
    switch(address) {
        // RAM Enable
        case 0x0000 ... 0x1FFF: {
            RAMEnabled = (value & 0x0F) == 0x0A;
            break;
        }
        // ROM bank number
        case 0x2000 ... 0x3FFF: {
            Uint8 bank = value & 0x1F;
            // Bank 0 becomes bank 1
            if (bank == 0) bank = 1;
            romBank = (romBank & 0x60) | bank;
            break;
        }
        // RAM bank or upper ROM bank bits
        case 0x4000 ... 0x5FFF: {
            Uint8 bits = value & 0x03;
            if (bankingMode == 0) {
                romBank = (bits << 5) | (romBank & 0x1F);
            }
            else {
                ramBank = bits;
            }
            break;
        }
        // Banking mode select
        case 0x6000 ... 0x7FFF: {
            bankingMode = value & 0x01;
            break;
        }
    }
}

void MBC_Write(Uint16 address, Uint8 value) {
    switch (currentMBC) {
        case MBC1: MBC_WriteMBC1(address, value); break;
        default: return; break;
    }
}

void MBC_SetMBC() {
    currentMBC = MBC_GetMBC();
}

Uint16 MBC_GetROMOffset() {
    return (0x4000 * romBank);
}

Uint16 MBC_GetRAMOffset() {
    return (0x2000 * ramBank);
}
