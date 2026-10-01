// Memory and registers

typedef struct registers {
    struct {
        union {
            struct {
                unsigned char f;
                unsigned char a;
            };
            unsigned short af;
        };
    };

    struct {
        union {
            struct {
                unsigned char c;
                unsigned char b;
            };
            unsigned short bc;
        };
    };

    struct {
        union {
            struct {
                unsigned char e;
                unsigned char d;
            };
            unsigned short de;
        };
    };

    struct {
        union {
            struct {
                unsigned char l;
                unsigned char h;
            };
            unsigned short hl;
        };
    };

    unsigned short pc;
    unsigned short sp;

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

void Registers_SetFlag(enum flags flag, unsigned int value);

unsigned int Registers_GetFlag(enum flags flag);
