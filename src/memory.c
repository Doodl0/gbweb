#include <stdint.h>
#include <stdio.h>
#include <SDL3/SDL_log.h>
#include <memory.h>

memoryBus memory;
unsigned char ime = 0;
static FILE* log;

void Memory_Write(unsigned short address, unsigned short value) {
    memory.memory[address] = value;
}

unsigned short Memory_Read(unsigned short address) {
    return memory.memory[address];
}

void Memory_Set(unsigned char* buffer, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        Memory_Write(i, buffer[i]);
    }
    SDL_Log("ROM %.16s loaded into GB memory, buffer %p size %zu", (&memory.memory[0]) + 0x134, &memory.memory, size);
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

void Memory_LogDeinit() {
    fclose(log);
}
