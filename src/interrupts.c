#include <interrupts.h>
#include <memory.h>
#include <SDL3/SDL_stdinc.h>

// Check if an individual flag is enabled
Uint8 Interrupts_Enabled(interruptsFlag flag) {
    Uint8 interruptsEnableRegister = Memory_Read(0xFFFF);
    switch (flag) {
        case INTERRUPTS_VBLANK: return (interruptsEnableRegister & 1); break;
        case INTERRUPTS_LCDSTATUS: return (interruptsEnableRegister & 2) >> 1; break;
        case INTERRUPTS_TIMEROVERFLOW: return (interruptsEnableRegister & 4) >> 2; break;
        case INTERRUPTS_SERIALLINK: return (interruptsEnableRegister & 8) >> 3; break;
        case INTERRUPTS_JOYPADPRESS: return (interruptsEnableRegister & 16) >> 4; break;
    }
}

// Set an individual flag
void Interrupts_FlagSet(interruptsFlag flag, Uint8 value) {
    Uint8 interruptsFlagsRegister = Memory_Read(0xFF0F);
    // If value is 1, set
    if (value) {
        switch (flag) {
            case INTERRUPTS_VBLANK: interruptsFlagsRegister |= 1; break;
            case INTERRUPTS_LCDSTATUS: interruptsFlagsRegister |= 1 << 1; break;
            case INTERRUPTS_TIMEROVERFLOW: interruptsFlagsRegister |= 1 << 2; break;
            case INTERRUPTS_SERIALLINK: interruptsFlagsRegister |= 1 << 3; break;
            case INTERRUPTS_JOYPADPRESS: interruptsFlagsRegister |= 1 << 4; break;
        }
        Memory_Write(0xFF0F, interruptsFlagsRegister & 0x1F);
    }
    // Else value is 0, so reset
    else {
        switch (flag) {
            case INTERRUPTS_VBLANK: interruptsFlagsRegister &= ~(1); break;
            case INTERRUPTS_LCDSTATUS: interruptsFlagsRegister &= ~(1 << 1); break;
            case INTERRUPTS_TIMEROVERFLOW: interruptsFlagsRegister &= ~(1 << 2); break;
            case INTERRUPTS_SERIALLINK: interruptsFlagsRegister &= ~(1 << 3); break;
            case INTERRUPTS_JOYPADPRESS: interruptsFlagsRegister &= ~(1 << 4); break;
        }
        Memory_Write(0xFF0F, interruptsFlagsRegister & 0x1F);
    }
}
