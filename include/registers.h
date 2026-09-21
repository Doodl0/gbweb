#include <stdbool.h>

// Registers
// AF, BC, DE, and HL are grouped as they are often utilised together, as virtual 16 bit registers
struct registers {
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

} extern registers;

enum flags {
    ZERO,
    SUBTRACT,
    HALF_CARRY,
    CARRY
};

// 8-bit registers
enum r8Enum {
    A, B, C, D, E, H, L, HL8
};
// Register pairs
enum r16Enum {
    BC, DE, HL, SP, AF
};
// Conditions
enum ccEnum {
    NZ, Z, NC, CA
};
