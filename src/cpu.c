#include <registers.h>
#include <memory.h>
#include <SDL3/SDL.h>

// Function for missing instruction
static void MissingInstruction(char *instruction) {
    SDL_Log("Missing function %s", instruction);
    SDL_Quit();
}

// Memory and registers
struct registers registers;
struct flagsRegister flagsRegister;
unsigned short pc;
struct memoryBus memoryBus;

// Modifiying registers based on enums
void WriteToR8(enum r8Enum target, unsigned char value) {
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

unsigned char ReadFromR8(enum r8Enum r) {
    unsigned char value;
    switch (r) {
        case A:
            value = registers.a;
            break;
        case B:
            value = registers.b;
            break;
        case C:
            value = registers.c;
            break;
        case D:
            value = registers.d;
            break;
        case E:
            value = registers.e;
            break;
        case H:
            value = registers.h;
            break;
        case L:
            value = registers.l;
            break;
    }
    return value;
}

// Value tables

// 8-bit registers
static enum r8Enum Table_r(unsigned int index) {
    enum r8Enum target = A;
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
            target = HL8;
            break;
        case 7:
            target = A;
            break;
    }
    return target;
}
// Register pairs featuring SP
static enum r16Enum Table_rp(unsigned int index) {
    enum r16Enum target = BC;
    switch (index) {
        case 0:
            target = BC;
            break;
        case 1:
            target = DE;
            break;
        case 2:
            target = HL;
            break;
        case 3:
            target = SP;
            break;
        }
        return target;
}
// Register pairs featuring AF
static enum r16Enum Table_rp2(unsigned int index) {
    enum r16Enum target = BC;
    switch (index) {
        case 0:
            target = BC;
            break;
        case 1:
            target = DE;
            break;
        case 2:
            target = HL;
            break;
        case 3:
            target = AF;
            break;
        }
        return target;
}
//
static enum ccEnum Table_cc(unsigned int index) {
    enum ccEnum target = NZ;
    switch (index) {
        case 0:
            target = NZ;
            break;
        case 1:
            target = Z;
            break;
        case 2:
            target = NC;
            break;
        case 3:
            target = CA;
            break;
        }
        return target;
}

// Instructions
static void ADC_A(unsigned char value) {
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

static void ADD_A(unsigned char value) {
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

static void SUB(unsigned char value) {
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

static void SBC_A(unsigned char value) {
    // Subtract carry flag and the input value from register A
    registers.a = registers.a - value - (unsigned char)flagsRegister.carry;

    // Check if overflown from bit 7
    if (value > registers.a) flagsRegister.carry = true;
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) flagsRegister.half_carry = true;
    // Check if 0
    if (registers.a == 0) flagsRegister.zero = true;
    // Is a subtract to set to false
    flagsRegister.subtract = true;
}

static void AND(unsigned char value) {
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

static void XOR(unsigned char value) {
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

static void OR(unsigned char value) {
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

static void CP(unsigned char value) {
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

// Instruction tables

// Arithmetic/logic operations
static void Table_alu(unsigned int index, unsigned char value) {
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
// alu table but accepts a register input, then passes value to main alu function
static void Table_alu_register(unsigned int index, enum r8Enum r) {
        if (r == HL8) {
            Table_alu(index, memoryBus.memory[registers.hl]);
        }
        else {
            Table_alu(index, ReadFromR8(r));
        }
}
// Rotation/shift operations
static void Table_rot(unsigned int index, enum r8Enum r) {
    unsigned char value = 0;
    if (r == HL8) {
        value = memoryBus.memory[registers.hl];
    }
    else {
        value = ReadFromR8(r);
    }

    switch (index) {
        case 0:
            MissingInstruction("RLC(value)");
            break;
        case 1:
            MissingInstruction("RRC(value)");
            break;
        case 2:
            MissingInstruction("RL(value)");
            break;
        case 3:
            MissingInstruction("RR(value)");
            break;
        case 4:
            MissingInstruction("SLA(value)");
            break;
        case 5:
            MissingInstruction("SRA(value)");
            break;
        case 6:
            MissingInstruction("SWAP(value)");
            break;
        case 7:
            MissingInstruction("SRL(value)");
            break;
    }
}

// Make sure all instructions are converted to a full 4 bytes first
void CPU_ExecuteInstruction(unsigned short address) {
    // Prefix CB
    if (memoryBus.memory[address] == (unsigned char)0xCB) {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        // Uses next byte as first byte is prefix
        unsigned char x = memoryBus.memory[address + 1] >> 6;
        unsigned char y = (memoryBus.memory[address + 1] >> 3) & 0x07;
        unsigned char z = memoryBus.memory[address + 1] & 0x07;

        switch (x) {
            // Roll/shift register or memory location
            case 0:
                Table_rot(y, Table_r(z));
                break;

            // Test bit
            case 1:
                MissingInstruction("BIT y, r[z]");
                break;

            // Reset bit
            case 2:
                MissingInstruction("RES y, r[z]");
                break;

            // Set bit
            case 3:
                MissingInstruction("SET y, r[z]");
                break;
        }
    }
    // No prefix
    else {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        unsigned char x = memoryBus.memory[address] >> 6;
        unsigned char y = (memoryBus.memory[address] >> 3) & 0x07;
        unsigned char z = memoryBus.memory[address] & 0x07;
        unsigned char p = y >> 1;
        unsigned char q = y % 2;

        switch (x) {
            case 0:
                switch (z) {
                    // Relative jumps and assorted ops
                    case 0:
                        switch (y) {
                            case 0:
                                MissingInstruction("NOP");
                                break;
                            case 1:
                                MissingInstruction("LD (nn), SP");
                                break;
                            case 2:
                                MissingInstruction("STOP");
                                break;
                            case 3:
                                MissingInstruction("JR d");
                                break;
                            case 4 ... 7:
                                MissingInstruction("JR cc[y-4], d");
                                break;
                        }
                    break;

                    // 16-bit load immediate/add
                    case 1:
                        switch (q) {
                            case 0:
                                MissingInstruction("LD rp[p], nn");
                                break;
                            case 1:
                                MissingInstruction("ADD HL, rp[p]");
                                break;
                        }
                    break;

                    // Indirect loading
                    case 2:
                        switch (q) {
                            case 0:
                                switch (p) {
                                    case 0:
                                        MissingInstruction("LD (BC), A");
                                        break;
                                    case 1:
                                        MissingInstruction("LD (DE), A");
                                        break;
                                    case 2:
                                        MissingInstruction("LD (HL+), A");
                                        break;
                                    case 3:
                                        MissingInstruction("LD (HL-), A");
                                        break;
                                }
                            case 1:
                                switch (p) {
                                    case 0:
                                        MissingInstruction("LD A, (BC)");
                                        break;
                                    case 1:
                                        MissingInstruction("LD A, (DE)");
                                        break;
                                    case 2:
                                        MissingInstruction("LD A, (HL+)");
                                        break;
                                    case 3:
                                        MissingInstruction("LD A, (HL-)");
                                        break;
                                }
                                break;
                        }
                    break;

                    // 16-bit INC/DEC
                    case 3:
                        switch (q) {
                            case 0:
                                MissingInstruction("INC rp[p]");
                                break;
                            case 1:
                                MissingInstruction("DEC rp[p]");
                                break;
                        }
                    break;

                    // 8-bit INC
                    case 4:
                       MissingInstruction("INC r[y]");
                    break;

                    // 8-bit DEC
                    case 5:
                       MissingInstruction("DEC r[y]");
                    break;

                    // 8-bit load immediate
                    case 6:
                       MissingInstruction("LD r[y], n");
                    break;

                    // Assorted operations on accumulator/flags
                    case 7:
                        switch (y) {
                            case 0:
                                MissingInstruction("RLCA");
                                break;
                            case 1:
                                MissingInstruction("RRCA");
                                break;
                            case 2:
                                MissingInstruction("RLA");
                                break;
                            case 3:
                                MissingInstruction("RRA");
                                break;
                            case 4:
                                MissingInstruction("DAA");
                                break;
                            case 5:
                                MissingInstruction("CPL");
                                break;
                            case 6:
                                MissingInstruction("SCF");
                                break;
                            case 7:
                                MissingInstruction("CCF");
                                break;
                        }
                    break;
                }
            break;

            case 1:
                // Exception (replaces LD (HL), (HL))
                if (z == 6) {
                    MissingInstruction("HALT");
                }
                // 8-bit loading
                else {
                    MissingInstruction("LD r[y], r[z]");
                }
            break;

            case 2:
                // Operate on accumulator and register/memory location
                Table_alu_register(y, Table_r(z));
            break;

            case 3:
                switch (z) {
                    // Conditional return, mem-mapped register loads and stack operations
                    case 0:
                        switch (y) {
                            case 0 ... 3:
                                MissingInstruction("RET cc[y]");
                                break;

                            case 4:
                                MissingInstruction("LD (0xFF00 + n), A");
                                break;

                            case 5:
                                MissingInstruction("ADD SP, d");
                                break;

                            case 6:
                                MissingInstruction("LD A, (0xFF00 + n)");
                                break;

                            case 7:
                                MissingInstruction("LD HL, SP+ d");
                                break;
                        }
                        break;

                    // POP & various ops
                    case 1:
                        switch (q) {
                            case 0:
                                MissingInstruction("POP rp2[p]");
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        MissingInstruction("RET");
                                        break;

                                    case 1:
                                        MissingInstruction("RETI");
                                        break;

                                    case 2:
                                        MissingInstruction("JP HL");
                                        break;

                                    case 3:
                                        MissingInstruction("LD SP, HL");
                                        break;
                                }
                                break;
                        }
                        break;

                    // Conditional jump
                    case 2:
                        switch (y) {
                            case 0 ... 3:
                                MissingInstruction("JP cc[y], nn");
                                break;

                            case 4:
                                MissingInstruction("LD (0xFF00+C), A");
                                break;

                            case 5:
                                MissingInstruction("LD (nn), A");
                                break;

                            case 6:
                                MissingInstruction("LD A, (0xFF00+C)");
                                break;

                            case 7:
                                MissingInstruction("LD A, (nn)");
                                break;
                        }
                    break;

                    // Assorted operations
                    case 3:
                        switch (y) {
                            case 0:
                                MissingInstruction("JP nn");
                                break;

                            case 6:
                                MissingInstruction("DI");
                                break;

                            case 7:
                                MissingInstruction("EI");
                                break;
                        }
                    break;

                    // Conditional call
                    case 4:
                        switch (y) {
                            case 0 ... 3:
                                MissingInstruction("CALL cc[y], nn");
                                break;
                        }
                    break;

                    // PUSH & various ops
                    case 5:
                        switch (q) {
                            case 0:
                                MissingInstruction("PUSH rp2[p]");
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        MissingInstruction("CALL nn");
                                        break;
                                }
                                break;
                        }
                    break;

                    // Operate on accumulator and immediate operand
                    case 6:
                        Table_alu(y, memoryBus.memory[address + 1]);
                    break;

                    // Restart
                    case 7:
                        MissingInstruction("RST y*8");
                    break;
                }
            break;
        }
    }
}

void CPU_Step() {
    unsigned char instructionByte = Memory_ReadByte(memoryBus, pc);

    CPU_ExecuteInstruction(pc);
}
