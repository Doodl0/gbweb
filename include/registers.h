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
} extern registers;

struct flagsRegister {
    bool zero;
    bool subtract;
    bool half_carry;
    bool carry;
};

enum registerEnum {
    A, B, C, D, E, H, L, HL
};

unsigned char ConvertFlagsRegisterToChar(struct flagsRegister flagsRegister);
struct flagsRegister ConvertCharToFlagRegister(unsigned char flags);
