#include "SDL3/SDL_video.h"
#include <registers.h>

struct Instruction {
    char* dissassembly;
    unsigned char operandLength;
    void* execute;
} typedef instruction;


/*
const struct instruction instructions[256] {

}
*/

struct registers registers;
struct flagsRegister flagsRegister;

void WriteToRegister(enum registerEnum target, unsigned char value) {
    switch (target) {
        case A:
            registers.a = value;
            break;
        case B:
            registers.b = value;
            break;
        case C:
            registers.c = value;
            break;
        case D:
            registers.d = value;
            break;
        case E:
            registers.e = value;
            break;
        case H:
            registers.h = value;
            break;
        case L:
            registers.l = value;
            break;
    }
}

void ADC_A(unsigned char value) {
    // Add carry flag, register A and the input value
    int result = registers.a + (value + (int)flagsRegister.carry);

    // Make sure the value is shortened to 8 bits
    registers.a = (unsigned char)(result & 0xff);

    // Check if overflown from bit 7
    if (result & 0xFF00) flagsRegister.carry = true;
    // Check if overflown from bit 3
    if ((registers.a & 0x0F) + (value & 0x0F) > 0x0F) flagsRegister.half_carry = true;
    // Check if 0
    if (result == 0) flagsRegister.zero = true;
    // Not a subtract to set to false
    flagsRegister.subtract = false;
}

void ADD_A(unsigned char value) {
    // Add register A and the input value
    int result = registers.a + value;

    // Make sure the value is shortened to 8 bits
    registers.a = (unsigned char)(result & 0xff);

    // Check if overflown from bit 7
    if (result & 0xFF00) flagsRegister.carry = true;
    // Check if overflown from bit 3
    if ((registers.a & 0x0F) + (value & 0x0F) > 0x0F) flagsRegister.half_carry = true;
    // Check if 0
    if (result == 0) flagsRegister.zero = true;
    // Not a subtract to set to false
    flagsRegister.subtract = false;
}

void SUB(unsigned char value) {
    registers.a = registers.a - value;

    // Check if overflown from bit 7
    if (value  > registers.a) flagsRegister.carry = true;
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) flagsRegister.half_carry = true;
    // Check if 0
    if (registers.a  == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = true;
}

void SBC_A(unsigned char value) {
    // Subtract carry flag and the input value from register A
    registers.a = registers.a - value - (unsigned char)flagsRegister.carry;

    // Check if overflown from bit 7
    if (value.a > registers.a) flagsRegister.carry = true;
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) flagsRegister.half_carry = true;
    // Check if 0
    if (registers.a == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = true;
}

void AND(unsigned char value) {
    // Bitwise AND of A and value
    registers.a = registers.a & value;

    // Check if overflown from bit 7
    flagsRegister.carry = false;
    // Check if overflown from bit 3
    flagsRegister.half_carry = true;
    // Check if 0
    if (registers.a == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = false;
}

void XOR(unsigned char value) {
    // Bitwise XOR of A and value
    registers.a = registers.a ^ value;

    // Check if overflown from bit 7
    flagsRegister.carry = false;
    // Check if overflown from bit 3
    flagsRegister.half_carry = false;
    // Check if 0
    if (registers.a == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = false;
}

void OR(unsigned char value) {
    // Bitwise XOR of A and value
    registers.a = registers.a | value;

    // Check if overflown from bit 7
    flagsRegister.carry = false;
    // Check if overflown from bit 3
    flagsRegister.half_carry = false;
    // Check if 0
    if (registers.a == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = false;
}

void CP(unsigned char value) {
    // Compare and set flags

    // Check if overflown from bit 7
    if (value  > registers.a) flagsRegister.carry = true;
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) flagsRegister.half_carry = true;
    // Check if 0
    if (registers.a  == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = true;
}

enum registerEnum Table_r(int index) {
    enum registerEnum target = A;
    switch (index) {
        case 0:
            target = B;
            break;
        case 1:
            target = C;
            break;
        case 2:
            target = D;
            break;
        case 3:
            target = E;
            break;
        case 4:
            target = H;
            break;
        case 5:
            target = L;
            break;
        case 6:
            target = HL;
            break;
        case 7:
            target = A;
            break;
    }
    return target;
}

void Table_alu(int index, unsigned char value) {
    switch (index) {
        case 0:
            ADD_A(value);
            break;
        case 1:
            ADC_A(value);
            break;
        case 2:
            SUB(value);
            break;
        case 3:
            SBC_A(value);
            break;
        case 4:
            AND(value);
            break;
        case 5:
            XOR(value);
            break;
        case 6:
            OR(value);
            break;
        case 7:
            CP(value);
            break;
    }
}

// Make sure all instructions are converted to a full 4 bytes first
void CPU_ExecuteInstruction(char instruction[4]) {

    // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
    char x = instruction[1] >> 6;
    char y = (instruction[1] >> 3) & 0x07;
    char z = instruction[1] & 0x07;
    char p = y >> 1;
    char q = y % 2;

    // Prefix CB
    if (instruction[0] == (char)0xCB) {
        switch (x) {
            // Roll/shift register or memory location
            case 0:
                // rot[y] r[z]
                break;

            // Test bit
            case 1:
                // BIT y, r[z]
                break;

            // Reset bit
            case 2:
                // RES y, r[z]
                break;

            // Set bit
            case 3:
                // SET y, r[z]
                break;
        }
    }
    // No prefix
    else {
        switch (x) {
            case 0:
                switch (z) {
                    // Relative jumps and assorted ops
                    case 0:
                        switch (y) {
                            case 0:
                                // NOP
                                break;
                            case 1:
                                // LD (nn), SP
                                break;
                            case 2:
                                // STOP
                                break;
                            case 3:
                                // JR d
                                break;
                            case 4 ... 7:
                                // JR cc[y-4], d
                                break;
                        }
                    break;

                    // 16-bit load immediate/add
                    case 1:
                        switch (q) {
                            case 0:
                                // LD rp[p], nn
                                break;
                            case 1:
                                // ADD HL, rp[p]
                                break;
                        }
                    break;

                    // Indirect loading
                    case 2:
                        switch (q) {
                            case 0:
                                switch (p) {
                                    case 0:
                                        // LD (BC), A
                                        break;
                                    case 1:
                                        // LD (DE), A
                                        break;
                                    case 2:
                                        // LD (HL+), A
                                        break;
                                    case 3:
                                        // LD (HL-), A
                                        break;
                                }
                            case 1:
                                switch (p) {
                                    case 0:
                                        // LD A, (BC)
                                        break;
                                    case 1:
                                        // LD A, (DE)
                                        break;
                                    case 2:
                                        // LD A, (HL+)
                                        break;
                                    case 3:
                                        // LD A, (HL-)
                                        break;
                                }
                                break;
                        }
                    break;

                    // 16-bit INC/DEC
                    case 3:
                        switch (q) {
                            case 0:
                                // INC rp[p]
                                break;
                            case 1:
                                // DEC rp[p]
                                break;
                        }
                    break;

                    // 8-bit INC
                    case 4:
                       // INC r[y]
                    break;

                    // 8-bit DEC
                    case 5:
                       // DEC r[y]
                    break;

                    // 8-bit load immediate
                    case 6:
                       // LD r[y], n
                    break;

                    // Assorted operations on accumulator/flags
                    case 7:
                        switch (y) {
                            case 0:
                                // RLCA
                                break;
                            case 1:
                                // RRCA
                                break;
                            case 2:
                                // RLA
                                break;
                            case 3:
                                // RRA
                                break;
                            case 4:
                                // DAA
                                break;
                            case 5:
                                // CPL
                                break;
                            case 6:
                                // SCF
                                break;
                            case 7:
                                // CCF
                                break;
                        }
                    break;
                }
            break;

            case 1:
                // Exception (replaces LD (HL), (HL))
                if (z == 6) {
                    // HALT
                }
                // 8-bit loading
                else {
                    // LD r[y], r[z]
                }
            break;

            case 2:
                // Operate on accumulator and register/memory location
                // alu[y] r[z]
            break;

            case 3:
                switch (z) {
                    // Conditional return, mem-mapped register loads and stack operations
                    case 0:
                        switch (y) {
                            case 0 ... 3:
                                // RET cc[y]
                                break;

                            case 4:
                                // LD (0xFF00 + n), A
                                break;

                            case 5:
                                // ADD SP, d
                                break;

                            case 6:
                                // LD A, (0xFF00 + n)
                                break;

                            case 7:
                                // LD HL, SP+ d
                                break;
                        }
                        break;

                    // POP & various ops
                    case 1:
                        switch (q) {
                            case 0:
                                // POP rp2[p]
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        // RET
                                        break;

                                    case 1:
                                        // RETI
                                        break;

                                    case 2:
                                        // JP HL
                                        break;

                                    case 3:
                                        // LD SP, HL
                                        break;
                                }
                                break;
                        }
                        break;

                    // Conditional jump
                    case 2:
                        switch (y) {
                            case 0 ... 3:
                                // JP cc[y], nn
                                break;

                            case 4:
                                // LD (0xFF00+C), A
                                break;

                            case 5:
                                // LD (nn), A
                                break;

                            case 6:
                                // LD A, (0xFF00+C)
                                break;

                            case 7:
                                // LD A, (nn)
                                break;
                        }
                    break;

                    // Assorted operations
                    case 3:
                        switch (y) {
                            case 0:
                                // JP nn
                                break;

                            case 6:
                                // DI
                                break;

                            case 7:
                                // EI
                                break;
                        }
                    break;

                    // Conditional call
                    case 4:
                        switch (y) {
                            case 0 ... 3:
                                // CALL cc[y], nn
                                break;
                        }
                    break;

                    // PUSH & various ops
                    case 5:
                        switch (q) {
                            case 0:
                                // PUSH rp2[p]
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        // CALL nn
                                        break;
                                }
                                break;
                        }
                    break;

                    // Operate on accumulator and immediate operand
                    case 6:
                        // alu[y] n
                    break;

                    // Restart
                    case 7:
                        // RST y*8
                    break;
                }
            break;
        }
    }
}
