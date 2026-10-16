#include <stdint.h>
#include <stdio.h>
#include <SDL3/SDL_log.h>
#include <memory.h>
#include <ppu.h>

memoryBus memory;
Uint8 ime = 0;
static FILE* log;

// Modifiying registers based on enums
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

void Registers_WriteR16(r16Enum target, Uint16 value) {
    switch (target) {
        case BC:
            registers.bc = value;
            break;
        case DE:
            registers.de = value;
            break;
        case HL:
            registers.hl = value;
            break;
        case SP:
            registers.sp = value;
            break;
        case AF:
            registers.af = value;
            break;
        case PC:
            registers.pc = value;
            break;
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

Uint16 Registers_ReadR16(r16Enum r) {
    Uint16 value;
    switch (r) {
        case BC:
            return registers.bc;
            break;
        case DE:
            return registers.de;
            break;
        case HL:
            return registers.hl;
            break;
        case SP:
            return registers.sp;
            break;
        case AF:
            return registers.af;
            break;
        case PC:
            return registers.pc;
            break;
    }
    return 0;
}

void Memory_Write(Uint16 address, Uint8 value) {
    // If in VRAM, set and also update tileset
    if((address >= 0x8000) && (address <= 0x9FFF)) {
        memory.VRAM[address& 0x1FFF] = value;
        if(address <= 0x97ff) PPU_UpdateTile(address);
    }
    else {
        memory.memory[address] = value;
    }
}

Uint8 Memory_Read(Uint16 address) {
    return memory.memory[address];
}

Uint8 Memory_ReadVRAM(Uint16 address) {
    return memory.VRAM[address];
}

void Memory_Set(Uint8* buffer, Uint32 size) {
    memcpy(&memory, buffer, 0x8000);
    SDL_Log("ROM %.16s loaded into GB memory, buffer %p size %u", (&memory.memory[0]) + 0x134, &memory.memory, size);
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
