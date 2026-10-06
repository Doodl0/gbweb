#include <SDL3/SDL_stdinc.h>
// Memory and registers

typedef struct registers {
    struct {
        union {
            struct {
                Uint8 f;
                Uint8 a;
            };
            Uint16 af;
        };
    };

    struct {
        union {
            struct {
                Uint8 c;
                Uint8 b;
            };
            Uint16 bc;
        };
    };

    struct {
        union {
            struct {
                Uint8 e;
                Uint8 d;
            };
            Uint16 de;
        };
    };

    struct {
        union {
            struct {
                Uint8 l;
                Uint8 h;
            };
            Uint16 hl;
        };
    };

    Uint16 pc;
    Uint16 sp;

} registersStruct;

extern registersStruct registers;

// 8-bit registers
typedef enum r8Enum {
    A, B, C, D, E, H, L, HL8
} r8Enum;
// Register pairs
typedef enum r16Enum {
    BC, DE, HL, SP, AF, PC
} r16Enum;
// Conditions
typedef enum ccEnum {
    NZ, Z, NC, CA
} ccEnum;

typedef enum flags {
    ZERO,
    SUBTRACT,
    HALF_CARRY,
    CARRY
} flags;

void Registers_SetFlag(enum flags flag, Uint8 value);

Uint8 Registers_GetFlag(enum flags flag);
