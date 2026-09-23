#include <stdio.h>
#include <memory.h>
#include <registers.h>

unsigned int ticks = 0;

void Log(struct memoryBus memory, struct registers registers) {
    #ifndef __EMSCRIPTEN__
    // Open a file in writing mode
    FILE* log = fopen("log.txt", "a");
    fprintf(log, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        registers.a, registers.f, registers.b, registers.c, registers.d, registers.e, registers.h, registers.l,
        registers.sp, registers.pc, memory.memory[registers.pc], memory.memory[registers.pc + 1], memory.memory[registers.pc + 2], memory.memory[registers.pc + 3]
    );
    fclose(log);
    #endif
}

void Tick(struct memoryBus memory, struct registers registers) {
    ticks++;
    Log(memory, registers);
}
