#include <stdbool.h>

#define ZERO_FLAG_BYTE_POSITION (char)7
#define SUBTRACT_FLAG_BYTE_POSITION (char)6
#define HALF_CARRY_FLAG_BYTE_POSITION (char)5
#define CARRY_FLAG_BYTE_POSITION (char)4

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
} typedef flags_register;

unsigned char ConvertFlagsRegisterToChar(flags_register flagsRegister) {
    return
    flagsRegister.zero << ZERO_FLAG_BYTE_POSITION |
    flagsRegister.subtract << SUBTRACT_FLAG_BYTE_POSITION |
    flagsRegister.half_carry << HALF_CARRY_FLAG_BYTE_POSITION |
    flagsRegister.carry << CARRY_FLAG_BYTE_POSITION;
}

flags_register ConvertCharToFlagRegister(unsigned char flags) {
    bool zero = (bool)(((flags >> ZERO_FLAG_BYTE_POSITION) & 1) != 0);
    bool subtract = (bool)(((flags >> SUBTRACT_FLAG_BYTE_POSITION) & 1) != 0);
    bool half_carry = (bool)(((flags >> HALF_CARRY_FLAG_BYTE_POSITION) & 1) != 0);
    bool carry = (bool)(((flags >> CARRY_FLAG_BYTE_POSITION) & 1) != 0);

    flags_register flagsRegister;
    flagsRegister.zero = zero;
    flagsRegister.subtract = subtract;
    flagsRegister.half_carry = half_carry;
    flagsRegister.carry = carry;
    return flagsRegister;
}
