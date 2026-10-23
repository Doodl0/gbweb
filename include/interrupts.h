#include <SDL3/SDL_stdinc.h>

typedef enum interruptsFlag {
    INTERRUPTS_VBLANK,
    INTERRUPTS_LCDSTATUS,
    INTERRUPTS_TIMEROVERFLOW,
    INTERRUPTS_SERIALLINK,
    INTERRUPTS_JOYPADPRESS
} interruptsFlag;

Uint8 Interrupts_Enabled(interruptsFlag flag);

void Interrupts_FlagSet(interruptsFlag flag, Uint8 value);
