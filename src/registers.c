#include "SDL3/SDL_stdinc.h"
#include <registers.h>
#define ZERO_FLAG_BIT_POSITION (char)(1<<7)
#define SUBTRACT_FLAG_BIT_POSITION (char)(1<<6)
#define HALF_CARRY_FLAG_BIT_POSITION (char)(1<<5)
#define CARRY_FLAG_BIT_POSITION (char)(1<<4)

// Registers
// AF, BC, DE, and HL are grouped as they are often utilised together, as virtual 16 bit registers
registersStruct registers;

// Value must be 1 or 0
void Registers_SetFlag(enum flags flag, Uint8 value) {
    if (value != 0) {
        switch(flag) {
            case ZERO:
                registers.f |= ZERO_FLAG_BIT_POSITION;
                break;
            case SUBTRACT:
                registers.f |= SUBTRACT_FLAG_BIT_POSITION;
                break;
            case HALF_CARRY:
                registers.f |= HALF_CARRY_FLAG_BIT_POSITION;
                break;
            case CARRY:
                registers.f |= CARRY_FLAG_BIT_POSITION;
                break;
        }
    }
    else {
        switch(flag) {
            case ZERO:
                registers.f &= ~(ZERO_FLAG_BIT_POSITION);
                break;
            case SUBTRACT:
                registers.f &= ~(SUBTRACT_FLAG_BIT_POSITION);
                break;
            case HALF_CARRY:
                registers.f &= ~(HALF_CARRY_FLAG_BIT_POSITION);
                break;
            case CARRY:
                registers.f &= ~(CARRY_FLAG_BIT_POSITION);
                break;
        }
    }
}

Uint8 Registers_GetFlag(enum flags flag) {
    unsigned int bit;
    switch(flag) {
        case ZERO:
            bit = (registers.f & (ZERO_FLAG_BIT_POSITION)) >> 7;
            break;
        case SUBTRACT:
            bit = (registers.f & (SUBTRACT_FLAG_BIT_POSITION)) >> 6;
            break;
        case HALF_CARRY:
            bit = (registers.f & (HALF_CARRY_FLAG_BIT_POSITION)) >> 5;
            break;
        case CARRY:
            bit = (registers.f & (CARRY_FLAG_BIT_POSITION)) >> 4;
            break;
    }
    return bit;
}
