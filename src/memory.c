#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL_log.h>
#include <memory.h>

memoryBus memory;
Uint8 ime = 0;
static FILE* log;

void Memory_Write(Uint16 address, Uint8 value) {
    memory.memory[address] = value;
}

Uint16 Memory_Read(Uint16 address) {
    return memory.memory[address];
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
