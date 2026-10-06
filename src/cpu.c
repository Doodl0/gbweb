#include <SDL3/SDL_log.h>
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <memory.h>
#include <cycles.h>

void CPU_Quit() {
    Memory_LogDeinit();
    SDL_Quit();
    exit(1);
}

// Function for missing instruction
static void CPU_MissingInstruction(char *instruction) {
    SDL_Log("Missing function %s, %02X, pc = %02X", instruction, Memory_Read(registers.pc), registers.pc);
    CPU_Quit();
}

// Modifiying registers based on enums
void WriteToR8(r8Enum target, unsigned char value) {
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
            Memory_Write(registers.hl, value);
    }
}

void WriteToR16(r16Enum target, unsigned short value) {
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

unsigned char ReadFromR8(r8Enum r) {
    switch (r) {
        case A:
            return registers.a;
            break;
        case B:
            return registers.b;
            break;
        case C:
            return registers.c;
            break;
        case D:
            return registers.d;
            break;
        case E:
            return registers.e;
            break;
        case H:
            return registers.h;
            break;
        case L:
            return registers.l;
            break;
        case HL8:
            return Memory_Read(registers.hl);
    }
    return 0;
}

unsigned short ReadFromR16(r16Enum r) {
    unsigned short value;
    switch (r) {
        case BC:
            return registers.bc;
            break;
        case DE:
            return registers.de;
            break;
        case HL:
            return registers.hl;
            break;
        case SP:
            return registers.sp;
            break;
        case AF:
            return registers.af;
            break;
        case PC:
            return registers.pc;
            break;
    }
    return 0;
}

// Flag checks
bool ConditionCodeCheck(ccEnum cc) {
    switch(cc) {
        case Z:
            if (Registers_GetFlag(ZERO) == 1) return true;
            break;
        case NZ:
            if (Registers_GetFlag(ZERO) == 0) return true;
            break;
        case CA:
            if (Registers_GetFlag(CARRY) == 1) return true;
            break;
        case NC:
            if (Registers_GetFlag(CARRY) == 0) return true;
            break;
    }
    return false;
}

// Value tables

// 8-bit registers
static r8Enum Table_r(unsigned int index) {
    switch (index) {
        case 0:
            return B;
            break;
        case 1:
            return C;
            break;
        case 2:
            return D;
            break;
        case 3:
            return E;
            break;
        case 4:
            return H;
            break;
        case 5:
            return L;
            break;
        case 6:
            return HL8;
            break;
        case 7:
            return A;
            break;
    }
    return B;
}
// Register pairs featuring SP
static r16Enum Table_rp(unsigned int index) {
    switch (index) {
        case 0:
            return BC;
            break;
        case 1:
            return DE;
            break;
        case 2:
            return HL;
            break;
        case 3:
            return SP;
            break;
        }
        return BC;
}
// Register pairs featuring AF
static r16Enum Table_rp2(unsigned int index) {
    switch (index) {
        case 0:
            return BC;
            break;
        case 1:
            return DE;
            break;
        case 2:
            return HL;
            break;
        case 3:
            return AF;
            break;
        }
        return BC;
}
// Condition codes
static ccEnum Table_cc(unsigned int index) {
    switch (index) {
        case 0:
            return NZ;
            break;
        case 1:
            return Z;
            break;
        case 2:
            return NC;
            break;
        case 3:
            return CA;
            break;
        }
        return NZ;
}

// Instructions
static void ADC_A(unsigned char value) {
    unsigned char carry = Registers_GetFlag(CARRY);

    // Check if overflown from bit 3
    if ((registers.a & 0x0F) + (value & 0x0F) + carry > 0x0F) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Add carry flag, register A and the input value
    int result = registers.a + (value + carry);

    // Make sure the value is shortened to 8 bits
    registers.a = (unsigned char)(result & 0xFF);

    // Check if overflown from bit 7
    if (result & 0xFF00) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);
    // Check if 0
    if (registers.a == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void ADD_HL(unsigned short value) {
    // Add register A and the input value
    int result = registers.hl + value;

    // Check if overflown from bit 11
    if ((registers.hl & 0xFFF) + (value & 0xFFF) > 0xFFF) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Make sure the value is shortened to 8 bits
    registers.hl = (unsigned short)(result & 0xFFFF);

    // Check if overflown from bit 15
    if (result & 0xFFFF0000) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void ADD_A(unsigned char value) {
    // Add register A and the input value
    int result = registers.a + value;

    // Check if overflown from bit 3
    if ((registers.a & 0xF) + (value & 0xF) > 0xF) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Make sure the value is shortened to 8 bits
    registers.a = (unsigned char)(result & 0xFF);

    // Check if overflown from bit 7
    if (result & 0xFF00) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);
    // Check if 0
    if (registers.a) Registers_SetFlag(ZERO, 0);
    else Registers_SetFlag(ZERO, 1);
    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void AND(unsigned char value) {
    // Bitwise AND of A and value
    registers.a = registers.a & value;

    Registers_SetFlag(CARRY, 0);
    Registers_SetFlag(HALF_CARRY, 1);
    // Check if 0
    if (registers.a == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    Registers_SetFlag(SUBTRACT, 0);
}

static void CP(unsigned char value) {
    // Compare and set flags

    // Check if overflown from bit 7
    if (value > registers.a) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a - value == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract so set to true
    Registers_SetFlag(SUBTRACT, 1);
}

static void DEC_r8(r8Enum target) {
    unsigned char value = ReadFromR8(target) - 1;
    WriteToR8(target, value);
    // Check if overflown from bit 3
    if ((value + 1) & 0x0F) Registers_SetFlag(HALF_CARRY, 0);
    else Registers_SetFlag(HALF_CARRY, 1);
    // Check if 0
    if (value == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract so set to false
    Registers_SetFlag(SUBTRACT, 1);
}

static void DEC_r16(r16Enum target) {
    WriteToR16(target, ReadFromR16(target) - 1);
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

static void INC_r8(r8Enum target) {
    unsigned char value = ReadFromR8(target) + 1;
    WriteToR8(target, value);
    // Check if overflown from bit 3
    if (((value - 1) & 0x0F) == 0x0F) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (value == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void INC_r16(r16Enum target) {
    WriteToR16(target, ReadFromR16(target) + 1);
}

static unsigned short JP(unsigned short value) {
    return value;
}

static unsigned short JR(signed char value) {
    return JP((registers.pc + 2) + value);
}

// Copy from byte at address n16 into A
static void LD_A_n16(unsigned short value) {
    WriteToR8(A, Memory_Read(value));
}

// Copy from n8 into r8
static void LD_r8_n8(r8Enum r8Target, unsigned char value) {
    WriteToR8(r8Target, value);
}

// Copy from n8 into byte at address n16
static void LD_n16_n8(unsigned short target, unsigned char value) {
    Memory_Write(target, value);
}

// Copy from value into r16
static void LD_r16_n16(r16Enum r16, unsigned short value) {
    WriteToR16(r16, value);
}

// Copy from SP to memory
static void LD_n16_n16(unsigned short address, unsigned short value) {
    Memory_Write(address, (unsigned char)(registers.sp & 0xFF));
    Memory_Write(address + 1, (unsigned char)(registers.sp >> 8));
}

static void NOP() {}

static void OR(unsigned char value) {
    // Bitwise XOR of A and value
    registers.a = registers.a | value;

    // Check if overflown from bit 7
    Registers_SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    Registers_SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a  == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract to set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void POP(r16Enum r) {
    WriteToR16(r, Memory_Read(registers.sp) | (Memory_Read(registers.sp + 1) << 8));
    registers.sp += 2;
}

static void PUSH(unsigned short value) {
    registers.sp-=2;
    Memory_Write(registers.sp + 1, (unsigned char)((value & 0xFF00) >> 8));
    Memory_Write(registers.sp, (unsigned char)((value & 0x00FF)));
}

static unsigned short CALL(unsigned short value) {
    PUSH(registers.pc + 3);
    return JP(value);
}

static unsigned short RET() {
    unsigned short value = Memory_Read(registers.sp) |  (Memory_Read(registers.sp + 1) << 8);
    registers.sp += 2;
    return value;
}

static unsigned short RETI() {
    EI();
    return RET();
}

static unsigned short RST() {}

static void SBC_A(unsigned char value) {
    // Subtract carry flag and the input value from register A
    registers.a = registers.a - value - (unsigned char)Registers_GetFlag(CARRY);

    // Check if overflown from bit 7
    if (value > registers.a) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a  == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract so set to true
    Registers_SetFlag(SUBTRACT, 1);
}

static void SUB(unsigned char value) {
    // Check if overflown from bit 7
    if (value > registers.a) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    // Check if overflown from bit 3
    if ((value & 0x0F) > (registers.a & 0x0F)) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    registers.a = registers.a - value;

    // Check if 0
    if (registers.a  == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract so set to true
    Registers_SetFlag(SUBTRACT, 1);
}

static unsigned char SRL(unsigned char value) {
    if(value & 0x01) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    value >>= 1;

    if(value == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);

    Registers_SetFlag(HALF_CARRY, 0);
    Registers_SetFlag(SUBTRACT, 0);

    return value;
}

static unsigned char RR(unsigned char value) {
    unsigned char newValue = (value >> 1) | (Registers_GetFlag(CARRY) << 7);

    if((value & 0x01)) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    if(newValue == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);

    Registers_SetFlag(HALF_CARRY, 0);
    Registers_SetFlag(SUBTRACT, 0);

    return newValue;
}

static void RRA() {
    registers.a = RR(registers.a);
    Registers_SetFlag(ZERO, 0);
}

static void XOR(unsigned char value) {
    // Bitwise XOR of A and value
    registers.a = registers.a ^ value;

    // Check if overflown from bit 7
    Registers_SetFlag(CARRY, 0);
    // Check if overflown from bit 3
    Registers_SetFlag(HALF_CARRY, 0);
    // Check if 0
    if (registers.a  == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract to set to false
    Registers_SetFlag(SUBTRACT, 0);
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
static void Table_alu_register(unsigned int index, r8Enum r) {
    Table_alu(index, ReadFromR8(r));
}
// Rotation/shift operations
static void Table_rot(unsigned int index, r8Enum r) {
    unsigned char value = ReadFromR8(r);
    switch (index) {
        case 0:
            CPU_MissingInstruction("RLC(value)");
            break;
        case 1:
            CPU_MissingInstruction("RRC(value)");
            break;
        case 2:
            CPU_MissingInstruction("RL(value)");
            break;
        case 3:
            value = RR(value);
            break;
        case 4:
            CPU_MissingInstruction("SLA(value)");
            break;
        case 5:
            CPU_MissingInstruction("SRA(value)");
            break;
        case 6:
            CPU_MissingInstruction("SWAP(value)");
            break;
        case 7:
            value = SRL(value);
            break;
    }
    WriteToR8(r, value);
}

// Executes an instruction and returns new pc address
unsigned short CPU_ExecuteInstruction(unsigned short address) {
    unsigned char instr = Memory_Read(address);

    // Prefix CB
    if (instr == 0xCB) {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        // Uses next byte as first byte is prefix
        instr = Memory_Read(address + 1);
        unsigned char x = instr >> 6;
        unsigned char y = (instr >> 3) & 0x07;
        unsigned char z = instr & 0x07;

        switch (x) {
            // Roll/shift register or memory location
            case 0:
                Table_rot(y, Table_r(z));
                if (Table_r(z) == HL8) Cycle_Add(4);
                else Cycle_Add(2);
                break;

            // Test bit
            case 1:
                CPU_MissingInstruction("BIT y, r[z]");
                if (Table_r(z) == HL8) Cycle_Add(3);
                else Cycle_Add(2);
                break;

            // Reset bit
            case 2:
                CPU_MissingInstruction("RES y, r[z]");
                if (Table_r(z) == HL8) Cycle_Add(4);
                else Cycle_Add(2);
                break;

            // Set bit
            case 3:
                CPU_MissingInstruction("SET y, r[z]");
                if (Table_r(z) == HL8) Cycle_Add(4);
                else Cycle_Add(2);
                break;
        }
        return (address + 2);
    }
    // No prefix
    else {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        unsigned char x = instr >> 6;
        unsigned char y = (instr >> 3) & 0x07;
        unsigned char z = instr & 0x07;
        unsigned char p = y >> 1;
        unsigned char q = y % 2;
        unsigned char n = Memory_Read(address + 1);
        unsigned short nn = ((unsigned short)(n)) | (((unsigned short)Memory_Read(address + 2)) << 8);

        switch (x) {
            case 0:
                switch (z) {
                    // Relative jumps and assorted ops
                    case 0:
                        switch (y) {
                            case 0:
                                NOP();
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 1:
                                LD_n16_n16(nn, registers.sp);
                                Cycle_Add(5);
                                return (address + 3);
                                break;
                            case 2:
                                CPU_MissingInstruction("STOP");
                                return (address + 1);
                                break;
                            case 3:
                                return JR((signed char) n);
                                Cycle_Add(3);
                                break;
                            case 4 ... 7:
                                if (ConditionCodeCheck(Table_cc(y-4))) {
                                    Cycle_Add(3);
                                    return JR((signed char) n);
                                }
                                else {
                                    Cycle_Add(2);
                                    return (address + 2);
                                }
                                break;
                        }
                    break;

                    // 16-bit load immediate/add
                    case 1:
                        switch (q) {
                            case 0:
                                LD_r16_n16(Table_rp(p), nn);
                                Cycle_Add(3);
                                return (address + 3);
                                break;
                            case 1:
                                ADD_HL(ReadFromR16(Table_rp(p)));
                                Cycle_Add(2);
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
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                    case 1:
                                        LD_n16_n8(registers.de, registers.a);
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                    case 2:
                                        LD_n16_n8(registers.hl, registers.a);
                                        registers.hl++;
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                    case 3:
                                        LD_n16_n8(registers.hl, registers.a);
                                        registers.hl--;
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                }
                            case 1:
                                switch (p) {
                                    case 0:
                                        LD_r8_n8(A, Memory_Read(registers.bc));
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                    case 1:
                                        LD_r8_n8(A, Memory_Read(registers.de));
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                    case 2:
                                        LD_r8_n8(A, Memory_Read(registers.hl));
                                        registers.hl++;
                                        Cycle_Add(2);
                                        return (address + 1);
                                        break;
                                    case 3:
                                        LD_r8_n8(A, Memory_Read(registers.hl));
                                        registers.hl--;
                                        Cycle_Add(2);
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
                                Cycle_Add(2);
                                return (address + 1);
                                break;
                            case 1:
                                DEC_r16(Table_rp(p));
                                Cycle_Add(2);
                                return (address + 1);
                                break;
                        }
                    break;

                    // 8-bit INC
                    case 4:
                        INC_r8(Table_r(y));
                        Cycle_Add(1);
                        return (address + 1);
                    break;

                    // 8-bit DEC
                    case 5:
                        DEC_r8(Table_r(y));
                        Cycle_Add(1);
                        return (address + 1);
                    break;

                    // 8-bit load immediate
                    case 6:
                        LD_r8_n8(Table_r(y), n);
                        Cycle_Add(2);
                        return (address + 2);
                    break;

                    // Assorted operations on accumulator/flags
                    case 7:
                        switch (y) {
                            case 0:
                                CPU_MissingInstruction("RLCA");
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 1:
                                CPU_MissingInstruction("RRCA");
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 2:
                                CPU_MissingInstruction("RLA");
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 3:
                                RRA();
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 4:
                                CPU_MissingInstruction("DAA");
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 5:
                                CPU_MissingInstruction("CPL");
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 6:
                                CPU_MissingInstruction("SCF");
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                            case 7:
                                CPU_MissingInstruction("CCF");
                                Cycle_Add(1);
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

                    if ((Table_r(y) == HL8) || (Table_r(z) == HL8)) Cycle_Add(2);
                    else Cycle_Add(1);

                    return (address + 1);
                }
            break;

            case 2:
                // Operate on accumulator and register/memory location
                Table_alu_register(y, Table_r(z));

                if (Table_r(z) == HL8) Cycle_Add(2);
                else Cycle_Add(1);

                return (address + 1);
            break;

            case 3:
                switch (z) {
                    // Conditional return, mem-mapped register loads and stack operations
                    case 0:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) {
                                    Cycle_Add(5);
                                    return RET();
                                }
                                else {
                                    Cycle_Add(2);
                                    return (address + 1);
                                }
                                break;

                            case 4:
                                LD_n16_n8((0xFF00 + n), registers.a);
                                Cycle_Add(3);
                                return (address + 2);
                                break;

                            case 5:
                                CPU_MissingInstruction("ADD SP, d");
                                Cycle_Add(4);
                                return (address + 2);
                                break;

                            case 6:
                                LD_A_n16(0xFF00 + n);
                                Cycle_Add(3);
                                return (address + 2);
                                break;

                            case 7:
                                CPU_MissingInstruction("LD HL, SP+ d");
                                Cycle_Add(3);
                                return (address + 2);
                                break;
                        }
                        break;

                    // POP & various ops
                    case 1:
                        switch (q) {
                            case 0:
                                POP(Table_rp2(p));
                                Cycle_Add(3);
                                return (address + 1);
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        Cycle_Add(4);
                                        return RET();
                                        break;

                                    case 1:
                                        Cycle_Add(4);
                                        return RETI();
                                        break;

                                    case 2:
                                        Cycle_Add(1);
                                        return JP(registers.hl);
                                        break;

                                    case 3:
                                        Cycle_Add(2);
                                        registers.sp = registers.hl; // LD SP HL
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
                                if (ConditionCodeCheck(Table_cc(y))) {
                                    Cycle_Add(4);
                                    return JP(nn);
                                }
                                else {
                                    Cycle_Add(3);
                                    return (address + 3);
                                }
                                break;

                            case 4:
                                LD_n16_n8((0xFF00 + registers.c), registers.a);
                                Cycle_Add(2);
                                return (address + 1);
                                break;

                            case 5:
                                LD_n16_n8(nn, registers.a);
                                Cycle_Add(4);
                                return (address + 3);
                                break;

                            case 6:
                                LD_A_n16(0xFF00 + registers.c);
                                return (address + 1);
                                break;

                            case 7:
                                LD_A_n16(nn);
                                Cycle_Add(2);
                                return (address + 3);
                                break;
                        }
                    break;

                    // Assorted operations
                    case 3:
                        switch (y) {
                            case 0:
                                Cycle_Add(4);
                                return JP(nn);
                                break;

                            case 6:
                                DI();
                                Cycle_Add(1);
                                return (address + 1);
                                break;

                            case 7:
                                EI();
                                Cycle_Add(1);
                                return (address + 1);
                                break;
                        }
                    break;

                    // Conditional call
                    case 4:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) {
                                    Cycle_Add(6);
                                    return CALL(nn);
                                }
                                else {
                                    Cycle_Add(3);
                                    return (address + 3);
                                }
                                break;
                        }
                    break;

                    // PUSH & various ops
                    case 5:
                        switch (q) {
                            case 0:
                                PUSH(ReadFromR16(Table_rp2(p)));
                                Cycle_Add(4);
                                return (address + 1);
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        Cycle_Add(6);
                                        return CALL(nn);
                                        break;
                                }
                                break;
                        }
                    break;

                    // Operate on accumulator and immediate operand
                    case 6:
                        Table_alu(y, n);
                        Cycle_Add(2);
                        return (address + 2);
                    break;

                    // Restart
                    case 7:
                        CPU_MissingInstruction("RST y*8");
                        Cycle_Add(4);
                        return (address + 1);
                    break;
                }
            break;
        }
    }
    return (address + 1);
}

void CPU_Step() {
    Cycle_Start();
    #ifndef NDEBUG
    Memory_Log(memory, registers);
    #endif
    registers.pc = CPU_ExecuteInstruction(registers.pc);
    Cycle_Wait();
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

    memory.memory[0xFF44] = 0x90;

    #ifndef NDEBUG
    Memory_LogInit();
    #endif
}

void CPU_ListInstructions() {
    for (unsigned char instr = 0; instr < 0xFF; instr ++) {
        Cycle_Start();
        memory.memory[0] = instr;
        registers.pc = 0;
        registers.pc = CPU_ExecuteInstruction(registers.pc);
        SDL_Log("Instr %02X, Cycles %u, Bytes %i", instr, Cycle_Get(), registers.pc);
    }
}
