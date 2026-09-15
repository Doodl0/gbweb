#include <stdbool.h>

#define ZERO_FLAG_BYTE_POSITION (char)7
#define SUBTRACT_FLAG_BYTE_POSITION (char)6
#define HALF_CARRY_FLAG_BYTE_POSITION (char)5
#define CARRY_FLAG_BYTE_POSITION (char)4

struct flagsRegister {
    bool zero;
    bool subtract;
    bool half_carry;
    bool carry;
};

unsigned char ConvertFlagsRegisterToChar(struct flagsRegister flagsRegister) {
    return
    flagsRegister.zero << ZERO_FLAG_BYTE_POSITION |
    flagsRegister.subtract << SUBTRACT_FLAG_BYTE_POSITION |
    flagsRegister.half_carry << HALF_CARRY_FLAG_BYTE_POSITION |
    flagsRegister.carry << CARRY_FLAG_BYTE_POSITION;
}

struct flagsRegister ConvertCharToFlagRegister(unsigned char flags) {
    bool zero = (bool)(((flags >> ZERO_FLAG_BYTE_POSITION) & 1) != 0);
    bool subtract = (bool)(((flags >> SUBTRACT_FLAG_BYTE_POSITION) & 1) != 0);
    bool half_carry = (bool)(((flags >> HALF_CARRY_FLAG_BYTE_POSITION) & 1) != 0);
    bool carry = (bool)(((flags >> CARRY_FLAG_BYTE_POSITION) & 1) != 0);

    struct flagsRegister flagsRegister;
    flagsRegister.zero = zero;
    flagsRegister.subtract = subtract;
    flagsRegister.half_carry = half_carry;
    flagsRegister.carry = carry;
    return flagsRegister;
}
