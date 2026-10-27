#include <SDL3/SDL_stdinc.h>
#include <memory.h>
#include <cycles.h>
#include <interrupts.h>

Uint32 cyclesElapsed = 0;
Uint32 clockMain = 0;
Uint32 clockDiv = 0;

Uint8 tacThreshold = 0;

Uint8 Timers_Register_DIV() {
    return Memory_Read(0xFF04);
}

Uint8 Timers_Register_TIMA() {
    return Memory_Read(0xFF05);
}

Uint8 Timers_Register_TMA() {
    return Memory_Read(0xFF06);
}

Uint8 Timers_Register_TAC() {
    return Memory_Read(0xFF07);
}

void Timers_DIV_Increment(Uint8 amount) {
    Uint8 newAmount = Memory_Read(0xFF04) + amount;
    Memory_Write(0xFF04, newAmount);
}

void Timers_TIMA_Increment(Uint8 amount) {
    Uint16 newAmount = (Uint16)Memory_Read(0xFF05) + (Uint16)amount;

    // Overflow interrupt and refill
    if (newAmount > 0xFF) {
        Interrupts_FlagSet(INTERRUPTS_TIMEROVERFLOW, 1);
        // Refill with the Modulo
        Memory_Write(0xFF05, Timers_Register_TMA());
    }
    else {
        Memory_Write(0xFF05, newAmount & 0xFF);
    }
}

void Timers_Step(Uint8 cycles) {
    cyclesElapsed += cycles;

    if (cyclesElapsed >= 16) {

        cyclesElapsed -= 16;
        clockMain += 1;

        // DIV register increments at 1/16th of clock speed
        clockDiv++;
        if (clockDiv == 16) {
            Timers_DIV_Increment(1);
            clockDiv = 0;
        }
    }

    // If timers running
    if (Timers_Register_TAC() & 4) {
        // Check and set speed
        switch (Timers_Register_TAC() & 3) {
            case 0: tacThreshold = 64; break; // 256K
            case 1: tacThreshold =  1; break; // 4K
            case 2: tacThreshold =  4; break; // 16K
            case 3: tacThreshold = 16; break; // 64K
        }

        if (clockMain >= tacThreshold) {
            // Step the timer up by one
            Timers_TIMA_Increment(1);
            clockMain = 0;
        }
    }


}
