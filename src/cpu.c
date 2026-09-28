#include <SDL3/SDL_log.h>
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <cpu.h>
#include <cycles.h>

#define ZERO_FLAG_BIT_POSITION (char)(1<<7)
#define SUBTRACT_FLAG_BIT_POSITION (char)(1<<6)
#define HALF_CARRY_FLAG_BIT_POSITION (char)(1<<5)
#define CARRY_FLAG_BIT_POSITION (char)(1<<4)

// Memory and registers
struct registers registers;
struct memoryBus memoryBus;
unsigned char ime = 0;


void WriteMemory(unsigned short address, unsigned short value) {
    if (address == 0xFF80) SDL_Log("FF83");
    memoryBus.memory[address] = value;
}

unsigned short ReadMemory(unsigned short address) {
    if (address == 0xFF83) SDL_Log("FF83");
    return memoryBus.memory[address];
}

// Function for missing instruction
static void MissingInstruction(char *instruction) {
    SDL_Log("Missing function %s, %02X, pc = %02X", instruction, ReadMemory(registers.pc), registers.pc);
    SDL_Quit();
    exit(1);
}

// Value must be 1 or 0
void SetFlag(enum flags flag, unsigned int value) {
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

unsigned int GetFlag(enum flags flag) {
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

void SetMemory(unsigned char* buffer, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        WriteMemory(i, buffer[i]);
    }
    SDL_Log("ROM %.16s loaded into GB memory, buffer %p size %zu", (&memoryBus.memory[0]) + 0x134, &memoryBus.memory, size);
    CPU_Init();
}

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
        case HL8:
            WriteMemory(registers.hl, value);
    }
}

void WriteToR16(enum r16Enum target, unsigned short value) {
    switch (target) {
        case BC:
            registers.bc = value;
            break;
        case DE:
            registers.de = value;
            break;
        case HL:
            registers.hl = value;
            break;
        case SP:
            registers.sp = value;
            break;
        case AF:
            registers.af = value;
            break;
        case PC:
            registers.pc = value;
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
        case HL8:
            value = 0xFF;
            SDL_Log("%02X, %04X, %02X, %02X", value, registers.hl, registers.h, registers.l);
    }
    return value;
}

unsigned short ReadFromR16(enum r16Enum r) {
    unsigned short value;
    switch (r) {
        case BC:
            value = registers.bc;
            break;
        case DE:
            value = registers.de;
            break;
        case HL:
            value = registers.hl;
            break;
        case SP:
            value = registers.sp;
            break;
        case AF:
            value = registers.af;
            break;
        case PC:
            value = registers.pc;
            break;
    }
    return value;
}

// Flag checks
bool ConditionCodeCheck(enum ccEnum cc) {
    switch(cc) {
        case Z:
            if (GetFlag(ZERO) == 1) return true;
            break;
        case NZ:
            if (GetFlag(ZERO) == 0) return true;
            break;
        case CA:
            if (GetFlag(CARRY) == 1) return true;
            break;
        case NC:
            if (GetFlag(CARRY) == 0) return true;
            break;
    }
    return false;
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
            //SDL_Log("%02X, %04X, %02X", registers.hl, registers.h, registers.l);

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
    int result = registers.a + (value + (unsigned char)GetFlag(CARRY));

    // Make sure the value is shortened to 8 bits
    registers.a = (unsigned char)(result & 0xff);

    // Check if overflown from bit 7
    if (result & 0xFF00) SetFlag(CARRY, 1);
    else SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    if ((registers.a & 0x0F) + (value & 0x0F) > 0x0F) SetFlag(HALF_CARRY, 1);
    else SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (result == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Not a subtract so set to false
    SetFlag(SUBTRACT, 0);
}

static void ADD_A(unsigned char value) {
    // Add register A and the input value
    int result = registers.a + value;

    // Check if overflown from bit 3
    if ((registers.a & 0xF) + (value & 0xF) > 0xF) SetFlag(HALF_CARRY, 1);
    else SetFlag(HALF_CARRY, 0);

    // Make sure the value is shortened to 8 bits
    registers.a = (unsigned char)(result & 0xff);

    // Check if overflown from bit 7
    if (result & 0xFF00) SetFlag(CARRY, 1);
    else SetFlag(CARRY, 0);
    // Check if 0
    if (registers.a) SetFlag(ZERO, 0);
    else SetFlag(ZERO, 1);
    // Not a subtract so set to false
    SetFlag(SUBTRACT, 0);
}

static void AND(unsigned char value) {
    // Bitwise AND of A and value
    registers.a = registers.a & value;

    SetFlag(CARRY, 0);
    SetFlag(HALF_CARRY, 1);
    // Check if 0
    if (registers.a == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    SetFlag(SUBTRACT, 0);
}

static void CP(unsigned char value) {
    // Compare and set flags

    // Check if overflown from bit 7
    if (value  > registers.a) SetFlag(CARRY, 1);
    else SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) SetFlag(HALF_CARRY, 1);
    else SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a - value  == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Is a subtract so set to true
    SetFlag(SUBTRACT, 1);
}

static void DEC_r8(enum r8Enum target) {
    unsigned char value = ReadFromR8(target) - 1;
    WriteToR8(target, value);
    // Check if overflown from bit 3
    if ((value + 1) & 0x0F) SetFlag(HALF_CARRY, 0);
    else SetFlag(HALF_CARRY, 1);
    // Check if 0
    if (value == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Is a subtract so set to false
    SetFlag(SUBTRACT, 1);
}

static void DEC_r16(enum r16Enum target) {
    unsigned short value = ReadFromR16(target) - 1;
    WriteToR16(target, value);
}

static void DI() {
    ime = 0;
}

static void EI() {
    ime = 1;
}

static void HALT() {
    if (ime) {
         SDL_Log("HALT here");
    }
    else {
        return;
    }
}

static void INC_r8(enum r8Enum target) {
    unsigned char value = ReadFromR8(target) + 1;
    WriteToR8(target, value);
    // Check if overflown from bit 3
    if (((value - 1) & 0x0F) == 0x0F) SetFlag(HALF_CARRY, 1);
    else SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (value == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Not a subtract so set to false
    SetFlag(SUBTRACT, 0);
}

static void INC_r16(enum r16Enum target) {
    unsigned short value = ReadFromR16(target) + 1;
    WriteToR16(target, value);
}

static unsigned short JP(unsigned short value) {
    return value;
}

static unsigned short JR(signed char value) {
    return JP((registers.pc + 2) + value);
}

// Copy from byte at address n16 into A
static void LD_A_n16(unsigned short value) {
    WriteToR8(A, ReadMemory(value));
}

// Copy from n8 into r8
static void LD_r8_n8(enum r8Enum r8Target, unsigned char value) {
    WriteToR8(r8Target, value);
}

// Copy from n8 into byte at address n16
static void LD_n16_n8(unsigned short target, unsigned char value) {
    WriteMemory(target, value);
}

// Copy from value into r16
static void LD_r16_n16(enum r16Enum r16, unsigned short value) {
    WriteToR16(r16, value);
}

static void NOP() {}

static void OR(unsigned char value) {
    // Bitwise XOR of A and value
    registers.a = registers.a | value;

    // Check if overflown from bit 7
    SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a  == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Is a subtract to set to false
    SetFlag(SUBTRACT, 0);
}

static void POP(enum r16Enum r) {
    unsigned short value =  ReadMemory(registers.sp) |  (ReadMemory(registers.sp + 1) << 8);
    registers.sp += 2;
    WriteToR16(r, value);
}

static void PUSH(unsigned short value) {
    registers.sp-=2;
    WriteMemory(registers.sp + 1, (unsigned char)((value & 0xFF00) >> 8));
    WriteMemory(registers.sp, (unsigned char)((value & 0x00FF)));
}

static unsigned short CALL(unsigned short value) {
    PUSH(registers.pc + 3);
    return JP(value);
}

static unsigned short RET() {
    unsigned short value =  ReadMemory(registers.sp) |  (ReadMemory(registers.sp + 1) << 8);
    registers.sp += 2;
    return value;
}

static unsigned short RETI() {
    EI();
    return RET();
}

static void SBC_A(unsigned char value) {
    // Subtract carry flag and the input value from register A
    registers.a = registers.a - value - (unsigned char)GetFlag(CARRY);

    // Check if overflown from bit 7
    if (value  > registers.a) SetFlag(CARRY, 1);
    else SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) SetFlag(HALF_CARRY, 1);
    else SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a  == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Is a subtract so set to true
    SetFlag(SUBTRACT, 1);
}

static void SUB(unsigned char value) {
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) SetFlag(HALF_CARRY, 1);
    else SetFlag(HALF_CARRY, 0);

    registers.a = registers.a - value;

    // Check if overflown from bit 7
    if (value  > registers.a) SetFlag(CARRY, 1);
    else SetFlag(CARRY, 0);
    // Check if 0
    if (registers.a  == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Is a subtract so set to true
    SetFlag(SUBTRACT, 1);
}

static void XOR(unsigned char value) {
    // Bitwise XOR of A and value
    registers.a = registers.a ^ value;

    // Check if overflown from bit 7
    SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a  == 0) SetFlag(ZERO, 1);
    else SetFlag(ZERO, 0);
    // Is a subtract to set to false
    SetFlag(SUBTRACT, 0);
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
    Table_alu(index, ReadFromR8(r));
}
// Rotation/shift operations
static void Table_rot(unsigned int index, enum r8Enum r) {
    unsigned char value = ReadFromR8(r);

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

// Executes an instruction and returns new pc address
unsigned short CPU_ExecuteInstruction(unsigned short address) {
    // Prefix CB
    if (ReadMemory(address) == (unsigned char)0xCB) {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        // Uses next byte as first byte is prefix
        unsigned char x = ReadMemory(address + 1) >> 6;
        unsigned char y = (ReadMemory(address + 1) >> 3) & 0x07;
        unsigned char z = ReadMemory(address + 1) & 0x07;

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
        return (address + 2);
    }
    // No prefix
    else {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        unsigned char x = ReadMemory(address) >> 6;
        unsigned char y = (ReadMemory(address) >> 3) & 0x07;
        unsigned char z = ReadMemory(address) & 0x07;
        unsigned char p = y >> 1;
        unsigned char q = y % 2;
        unsigned short nn = ((unsigned short)(ReadMemory(address + 1))) | (((unsigned short)ReadMemory(address + 2)) << 8);

        switch (x) {
            case 0:
                switch (z) {
                    // Relative jumps and assorted ops
                    case 0:
                        switch (y) {
                            case 0:
                                NOP();
                                return (address + 1);
                                break;
                            case 1:
                                MissingInstruction("LD (nn), SP");
                                return (address + 3);
                                break;
                            case 2:
                                MissingInstruction("STOP");
                                return (address + 1);
                                break;
                            case 3:
                                return JR((signed char) ReadMemory(address + 1));
                                break;
                            case 4 ... 7:
                                if (ConditionCodeCheck(Table_cc(y-4))) return JR((signed char) ReadMemory(address + 1));
                                else return (address + 2);
                                break;
                        }
                    break;

                    // 16-bit load immediate/add
                    case 1:
                        switch (q) {
                            case 0:
                                LD_r16_n16(Table_rp(p), nn);
                                return (address + 3);
                                break;
                            case 1:
                                MissingInstruction("ADD HL, rp[p]");
                                return (address + 1);
                                break;
                        }
                    break;

                    // Indirect loading
                    case 2:
                        switch (q) {
                            case 0:
                                switch (p) {
                                    case 0:
                                        LD_n16_n8(registers.bc, registers.a);
                                        return (address + 1);
                                        break;
                                    case 1:
                                        LD_n16_n8(registers.de, registers.a);
                                        return (address + 1);
                                        break;
                                    case 2:
                                        LD_n16_n8(registers.hl, registers.a);
                                        registers.hl++;
                                        return (address + 1);
                                        break;
                                    case 3:
                                        LD_n16_n8(registers.hl, registers.a);
                                        registers.hl--;
                                        return (address + 1);
                                        break;
                                }
                            case 1:
                                switch (p) {
                                    case 0:
                                        LD_r8_n8(A, ReadMemory(registers.bc));
                                        return (address + 1);
                                        break;
                                    case 1:
                                        LD_r8_n8(A, ReadMemory(registers.de));
                                        return (address + 1);
                                        break;
                                    case 2:
                                        LD_r8_n8(A, ReadMemory(registers.hl));
                                        registers.hl++;
                                        return (address + 1);
                                        break;
                                    case 3:
                                        LD_r8_n8(A, ReadMemory(registers.hl));
                                        registers.hl--;
                                        return (address + 1);
                                        break;
                                }
                                break;
                        }
                    break;

                    // 16-bit INC/DEC
                    case 3:
                        switch (q) {
                            case 0:
                                INC_r16(Table_rp(p));
                                return (address + 1);
                                break;
                            case 1:
                                DEC_r16(Table_rp(p));
                                MissingInstruction("DEC rp[p]");
                                return (address + 1);
                                break;
                        }
                    break;

                    // 8-bit INC
                    case 4:
                        INC_r8(Table_r(y));
                       return (address + 1);
                    break;

                    // 8-bit DEC
                    case 5:
                        DEC_r8(Table_r(y));
                       return (address + 1);
                    break;

                    // 8-bit load immediate
                    case 6:
                        LD_r8_n8(Table_r(y), ReadMemory(address + 1));
                       return (address + 2);
                    break;

                    // Assorted operations on accumulator/flags
                    case 7:
                        switch (y) {
                            case 0:
                                MissingInstruction("RLCA");
                                return (address + 1);
                                break;
                            case 1:
                                MissingInstruction("RRCA");
                                return (address + 1);
                                break;
                            case 2:
                                MissingInstruction("RLA");
                                return (address + 1);
                                break;
                            case 3:
                                MissingInstruction("RRA");
                                return (address + 1);
                                break;
                            case 4:
                                MissingInstruction("DAA");
                                return (address + 1);
                                break;
                            case 5:
                                MissingInstruction("CPL");
                                return (address + 1);
                                break;
                            case 6:
                                MissingInstruction("SCF");
                                return (address + 1);
                                break;
                            case 7:
                                MissingInstruction("CCF");
                                return (address + 1);
                                break;
                        }
                    break;
                }
            break;

            case 1:
                // Exception (replaces LD (HL), (HL))
                if (z == 6 && y == 6) {
                    HALT();
                    return (address + 1);
                }
                // 8-bit loading
                else {
                    LD_r8_n8(Table_r(y), ReadFromR8(Table_r(z)));
                    return (address + 1);
                }
            break;

            case 2:
                // Operate on accumulator and register/memory location
                Table_alu_register(y, Table_r(z));
                return (address + 1);
            break;

            case 3:
                switch (z) {
                    // Conditional return, mem-mapped register loads and stack operations
                    case 0:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) return RET();
                                else return (address + 1);
                                break;

                            case 4:
                                LD_n16_n8((0xFF00 + address + 1), registers.a);
                                return (address + 2);
                                break;

                            case 5:
                                MissingInstruction("ADD SP, d");
                                return (address + 2);
                                break;

                            case 6:
                                LD_r8_n8(A, ReadMemory((0xFF00 + ReadMemory(address + 1))));
                                return (address + 2);
                                break;

                            case 7:
                                MissingInstruction("LD HL, SP+ d");
                                return (address + 2);
                                break;
                        }
                        break;

                    // POP & various ops
                    case 1:
                        switch (q) {
                            case 0:
                                POP(Table_rp2(p));
                                return (address + 1);
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        return RET();
                                        break;

                                    case 1:
                                        RETI();
                                        return (address + 1);
                                        break;

                                    case 2:
                                        return JP(registers.hl);
                                        break;

                                    case 3:
                                        MissingInstruction("LD SP, HL");
                                        return (address + 1);
                                        break;
                                }
                                break;
                        }
                        break;

                    // Conditional jump
                    case 2:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) return JP(nn);
                                else return (address + 3);
                                break;

                            case 4:
                                LD_n16_n8((0xFF00 + registers.c), registers.a);
                                return (address + 1);
                                break;

                            case 5:
                                LD_n16_n8(nn, registers.a);
                                return (address + 3);
                                break;

                            case 6:
                                LD_A_n16(0xFF00 + registers.c);
                                return (address + 1);
                                break;

                            case 7:
                                LD_A_n16(nn);
                                return (address + 3);
                                break;
                        }
                    break;

                    // Assorted operations
                    case 3:
                        switch (y) {
                            case 0:
                                return JP(nn);
                                break;

                            case 6:
                                DI();
                                return (address + 1);
                                break;

                            case 7:
                                EI();
                                return (address + 1);
                                break;
                        }
                    break;

                    // Conditional call
                    case 4:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) return CALL(nn);
                                else return (address + 3);
                                break;
                        }
                    break;

                    // PUSH & various ops
                    case 5:
                        switch (q) {
                            case 0:
                                PUSH(ReadFromR16(Table_rp2(p)));
                                return (address + 1);
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        return CALL(nn);
                                        break;
                                }
                                break;
                        }
                    break;

                    // Operate on accumulator and immediate operand
                    case 6:
                        Table_alu(y, ReadMemory(address + 1));
                        return (address + 2);
                    break;

                    // Restart
                    case 7:
                        MissingInstruction("RST y*8");
                        return (address + 1);
                    break;
                }
            break;
        }
    }
    return (address + 1);
}

void CPU_Step() {
    //SDL_Log("%04X",registers.pc);
    //if (registers.pc > 0x100) MissingInstruction("break");
    Tick(memoryBus, registers);

    registers.pc = CPU_ExecuteInstruction(registers.pc);
}

void CPU_Init() {
    registers.a = 0x01;
    registers.f = 0xB0;
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.sp = 0xFFFE;
    registers.pc = 0x0100;

    memoryBus.memory[0xFF44] = 0x90;
}
