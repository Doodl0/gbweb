#include "SDL3/SDL_stdinc.h"
#include <stdint.h>
#include <stdio.h>
#include <SDL3/SDL_log.h>
#include <memory.h>
#include <ppu.h>
#include <keypad.h>
#include <cycles.h>
#include <mbc.h>

memoryBus memory;
Uint8 ime = 0;
static FILE* log;

// Modifiying registers based on enums
// (8 bit registers are here because of (HL), which requires reading memory)
void Registers_WriteR8(r8Enum target, Uint8 value) {
    switch (target) {
        case A:
            registers.a = value;
            break;
        case B:
            registers.b = value;
            break;
        case C:
            registers.c = value;
            break;
        case D:
            registers.d = value;
            break;
        case E:
            registers.e = value;
            break;
        case H:
            registers.h = value;
            break;
        case L:
            registers.l = value;
            break;
        case HL8:
            Memory_Write(registers.hl, value);
    }
}

Uint8 Registers_ReadR8(r8Enum r) {
    switch (r) {
        case A:
            return registers.a;
            break;
        case B:
            return registers.b;
            break;
        case C:
            return registers.c;
            break;
        case D:
            return registers.d;
            break;
        case E:
            return registers.e;
            break;
        case H:
            return registers.h;
            break;
        case L:
            return registers.l;
            break;
        case HL8:
            return Memory_Read(registers.hl);
    }
    return 0;
}

void Memory_OAM_DMATransfer(Uint8 value) {
    Uint16 sourceAddress = value << 8;

    // Copy from 0xFE00 to 0xFE9F (OAM)
    for(Uint16 i = 0; i <= 0x9F; i++) {
        Memory_Write(0xFE00 + i, memory.memory[sourceAddress + i]);
    }

    // This takes 160 M cycles
    Cycles_AddM(160);
}

void Memory_Write(Uint16 address, Uint8 value) {
    // Don't allow writing to cartridge, instead set MBC things
    if ((address >= 0x0) && (address <= 0x8000)) {
        MBC_Write(address, value);
    }

    // External RAM
    else if ((address >= 0xA000) && (address <= 0xBFFF)) {
        memory.cartridgeRAM[MBC_GetRAMOffset() + (address & 0x1FFF)] = value;
    }

    // If in VRAM, set and also update tileset
    if ((address >= 0x8000) && (address <= 0x9FFF)) {
        memory.VRAM[address& 0x1FFF] = value;
        if (address <= 0x97FF) PPU_UpdateTile(address);
    }

    // Keypad I/O register
    else if (address == 0xFF00) Keypad_Write(value);

    // OAM DMA transfer
    else if (address == 0xFF46) Memory_OAM_DMATransfer(value);
    else memory.memory[address] = value;
}

Uint8 Memory_Read(Uint16 address) {
    // Switchable bank memory
    if ((address >= 0x4000) && (address < 0x8000)) {
        return memory.ROM[MBC_GetROMOffset() + (address & 0x3FFF)];
    }

    // External RAM
    else if ((address >= 0xA000) && (address <= 0xBFFF)) {
        return memory.cartridgeRAM[MBC_GetRAMOffset() + (address & 0x1FFF)];
    }

    // Read only PPU current scanline register
    else if (address == 0xFF44) return PPU_Line();
    // Keypad I/O register
    else if (address == 0xFF00) return Keypad_Read();

    // Serial control register
    else if (address == 0xFF01) return 0xFF;

    // Serial control register
    else if (address == 0xFF02) return 0xFF;

    else return memory.memory[address];
}

Uint8 Memory_ReadVRAM(Uint16 address) {
    return memory.VRAM[address];
}

Uint8 Memory_ReadROM(Uint16 address) {
    return memory.ROM[address];
}

void Memory_SetROMBank(Uint8 sourceBank, Uint8 destinationBank) {
    // Copy ROM bank to bank in memory
    memcpy(&memory.memory[0x4000 * destinationBank], &memory.ROM[0x4000 * sourceBank], 0x8000);
}

void Memory_SetRAMBank(Uint8 sourceBank) {
    // Copy RAM bank to bank in memory
    memcpy(&memory.externalRAM, &memory.cartridgeRAM[0x2000 * sourceBank], 0x2000);
}

void Memory_Set(Uint8* buffer, Uint32 size) {
    // Copy ROM
    memcpy(&memory.ROM, buffer, 0x400000);
    // Copy first 2 ROM banks into memory
    Memory_SetROMBank(0, 0);
    Memory_SetROMBank(1, 1);

    MBC_SetMBC();

    SDL_Log("ROM %.16s loaded into GB memory, buffer %p size", (&memory.memory[0]) + 0x134, &memory.memory);
}



void Memory_LogInit() {
    log = fopen("log.txt", "w");
}

void Memory_Log(memoryBus memory, registersStruct registers) {
    #ifndef __EMSCRIPTEN__
    // Open a file in writing mode
    fprintf(log, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        registers.a, registers.f, registers.b, registers.c, registers.d, registers.e, registers.h, registers.l,
        registers.sp, registers.pc, memory.memory[registers.pc], memory.memory[registers.pc + 1], memory.memory[registers.pc + 2], memory.memory[registers.pc + 3]
    );
    #endif
}

void Memory_Deinit() {
    fclose(log);
}
