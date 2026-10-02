#include <stdint.h>
#include <stdio.h>
#include <registers.h>

typedef struct memoryBus {
    unsigned char memory[0xFFFF];
} memoryBus;

extern memoryBus memory;
extern unsigned char ime;

void Memory_Write(unsigned short address, unsigned short value);

unsigned short Memory_Read(unsigned short address);

void Memory_Set(unsigned char* buffer, size_t size);

void Memory_LogInit();

void Memory_Log(memoryBus memory, registersStruct registers);

void Memory_LogDeinit();
