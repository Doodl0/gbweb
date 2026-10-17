#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <memory.h>
#include <cycles.h>

bool halted = false;
bool stopped = false;

void CPU_Quit() {
    Memory_Deinit();
    SDL_Quit();
    exit(1);
}

// Function for missing instruction
static void CPU_MissingInstruction(char *instruction) {
    SDL_Log("Missing function %s, %02X, pc = %02X", instruction, Memory_Read(registers.pc), registers.pc);
    CPU_Quit();
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
static r8Enum Table_r(Uint8 index) {
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
static r16Enum Table_rp(Uint8 index) {
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
static r16Enum Table_rp2(Uint8 index) {
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
static ccEnum Table_cc(Uint8 index) {
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
static void ADC_A(Uint8 value) {
    Uint8 carry = Registers_GetFlag(CARRY);

    // Check if overflown from bit 3
    if ((registers.a & 0x0F) + (value & 0x0F) + carry > 0x0F) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Add carry flag, register A and the input value
    int result = registers.a + (value + carry);

    // Make sure the value is shortened to 8 bits
    registers.a = (Uint8)(result & 0xFF);

    // Check if overflown from bit 7
    if (result & 0xFF00) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);
    // Check if 0
    if (registers.a == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void ADD_HL(Uint16 value) {
    // Add register A and the input value
    int result = registers.hl + value;

    // Check if overflown from bit 11
    if ((registers.hl & 0xFFF) + (value & 0xFFF) > 0xFFF) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Make sure the value is shortened to 8 bits
    registers.hl = (Uint16)(result & 0xFFFF);

    // Check if overflown from bit 15
    if (result & 0xFFFF0000) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void ADD_A(Uint8 value) {
    // Add register A and the input value
    int result = registers.a + value;

    // Check if overflown from bit 3
    if ((registers.a & 0xF) + (value & 0xF) > 0xF) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Make sure the value is shortened to 8 bits
    registers.a = (Uint8)(result & 0xFF);

    // Check if overflown from bit 7
    if (result & 0xFF00) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);
    // Check if 0
    if (registers.a) Registers_SetFlag(ZERO, 0);
    else Registers_SetFlag(ZERO, 1);
    // Not a subtract so set to false
    Registers_SetFlag(SUBTRACT, 0);
}

static void AND(Uint8 value) {
    // Bitwise AND of A and value
    registers.a = registers.a & value;

    Registers_SetFlag(CARRY, 0);
    Registers_SetFlag(HALF_CARRY, 1);
    // Check if 0
    if (registers.a == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    Registers_SetFlag(SUBTRACT, 0);
}

static void CP(Uint8 value) {
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
    Uint8 value = Registers_ReadR8(target) - 1;
    Registers_WriteR8(target, value);
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
    Registers_WriteR16(target, Registers_ReadR16(target) - 1);
}

static void DI() {
    ime = 0;
}

static void EI() {
    ime = 1;
}

static void HALT() {
    if (ime) {
         halted = true;
    }
    else {
        return;
    }
}

static void INC_r8(r8Enum target) {
    Uint8 value = Registers_ReadR8(target) + 1;
    Registers_WriteR8(target, value);
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
    Registers_WriteR16(target, Registers_ReadR16(target) + 1);
}

static Uint16 JP(Uint16 value) {
    return value;
}

static Uint16 JR(Sint8 value) {
    return JP((registers.pc + 2) + value);
}

// Copy from byte at address n16 into A
static void LD_A_n16(Uint16 value) {
    Registers_WriteR8(A, Memory_Read(value));
}

// Copy from n8 into r8
static void LD_r8_n8(r8Enum r8Target, Uint8 value) {
    Registers_WriteR8(r8Target, value);
}

// Copy from n8 into byte at address n16
static void LD_n16_n8(Uint16 target, Uint8 value) {
    Memory_Write(target, value);
}

// Copy from value into r16
static void LD_r16_n16(r16Enum r16, Uint16 value) {
    Registers_WriteR16(r16, value);
}

// Copy from SP to memory
static void LD_n16_n16(Uint16 address, Uint16 value) {
    Memory_Write(address, (Uint8)(registers.sp & 0xFF));
    Memory_Write(address + 1, (Uint8)(registers.sp >> 8));
}

static void NOP() {}

static void OR(Uint8 value) {
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
    // If AF, only write bits 7 to 4 into F
    if (r == AF) Registers_WriteR16(r, (Memory_Read(registers.sp) & 0xF0) | ((Memory_Read(registers.sp + 1) << 8)));
    else Registers_WriteR16(r, Memory_Read(registers.sp) | (Memory_Read(registers.sp + 1) << 8));
    registers.sp += 2;
}

static void PUSH(Uint16 value) {
    registers.sp-=2;
    Memory_Write(registers.sp + 1, (Uint8)((value & 0xFF00) >> 8));
    Memory_Write(registers.sp, (Uint8)((value & 0x00FF)));
}

static Uint16 CALL(Uint16 value) {
    PUSH(registers.pc + 3);
    return JP(value);
}

static Uint16 RET() {
    Uint16 value = Memory_Read(registers.sp) |  (Memory_Read(registers.sp + 1) << 8);
    registers.sp += 2;
    return value;
}

static Uint16 RETI() {
    EI();
    return RET();
}

static Uint16 RST(Uint8 vec) {
    PUSH(registers.pc + 1);
    return JP((Uint16)vec);
}

static void SBC_A(Uint8 value) {
    Uint8 carry = Registers_GetFlag(CARRY);

    // Check if overflown from bit 3
    if ((((value) & 0x0F) + carry) > (registers.a & 0x0F)) Registers_SetFlag(HALF_CARRY, 1);
    else Registers_SetFlag(HALF_CARRY, 0);

    // Check if overflown from bit 7
    if ((value + carry) > (registers.a)) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    // Subtract carry flag and the input value from register A
    registers.a = registers.a - value - carry;

    // Check if 0
    if (registers.a == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    // Is a subtract so set to true
    Registers_SetFlag(SUBTRACT, 1);
}

static void SUB(Uint8 value) {
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

static Uint8 SRL(Uint8 value) {
    if(value & 0x01) Registers_SetFlag(CARRY, 1);
    else Registers_SetFlag(CARRY, 0);

    value >>= 1;

    if(value == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);

    Registers_SetFlag(HALF_CARRY, 0);
    Registers_SetFlag(SUBTRACT, 0);

    return value;
}

static Uint8 SWAP(Uint8 value) {
    value = ((value & 0xF) << 4) | ((value & 0xF0) >> 4);

    if(value == 0) Registers_SetFlag(ZERO, 1);
    else Registers_SetFlag(ZERO, 0);
    Registers_SetFlag(CARRY, 0);
    Registers_SetFlag(HALF_CARRY, 0);
    Registers_SetFlag(SUBTRACT, 0);

    return value;
}

static Uint8 RR(Uint8 value) {
    Uint8 newValue = (value >> 1) | (Registers_GetFlag(CARRY) << 7);

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

static void XOR(Uint8 value) {
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
static void Table_alu(Uint8 index, Uint8 value) {
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
static void Table_alu_register(Uint8 index, r8Enum r) {
    Table_alu(index, Registers_ReadR8(r));
}
// Rotation/shift operations
static void Table_rot(Uint8 index, r8Enum r) {
    Uint8 value = Registers_ReadR8(r);
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
            value = SWAP(value);
            break;
        case 7:
            value = SRL(value);
            break;
    }
    Registers_WriteR8(r, value);
}

// Executes an instruction and returns new pc address
Uint16 CPU_ExecuteInstruction(Uint16 address) {
    Uint8 instr = Memory_Read(address);

    // Prefix CB
    if (instr == 0xCB) {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        // Uses next byte as first byte is prefix
        instr = Memory_Read(address + 1);
        Uint8 x = instr >> 6;
        Uint8 y = (instr >> 3) & 0x07;
        Uint8 z = instr & 0x07;

        switch (x) {
            // Roll/shift register or memory location
            case 0:
                Table_rot(y, Table_r(z));
                if (Table_r(z) == HL8) Cycles_AddM(4);
                else Cycles_AddM(2);
                break;

            // Test bit
            case 1:
                CPU_MissingInstruction("BIT y, r[z]");
                if (Table_r(z) == HL8) Cycles_AddM(3);
                else Cycles_AddM(2);
                break;

            // Reset bit
            case 2:
                CPU_MissingInstruction("RES y, r[z]");
                if (Table_r(z) == HL8) Cycles_AddM(4);
                else Cycles_AddM(2);
                break;

            // Set bit
            case 3:
                CPU_MissingInstruction("SET y, r[z]");
                if (Table_r(z) == HL8) Cycles_AddM(4);
                else Cycles_AddM(2);
                break;
        }
        return (address + 2);
    }
    // No prefix
    else {

        // Variables based on https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding Gamboy Z80 Opcodes.html
        Uint8 x = instr >> 6;
        Uint8 y = (instr >> 3) & 0x07;
        Uint8 z = instr & 0x07;
        Uint8 p = y >> 1;
        Uint8 q = y % 2;
        Uint8 n = Memory_Read(address + 1);
        Uint16 nn = ((Uint16)(n)) | (((Uint16)Memory_Read(address + 2)) << 8);

        switch (x) {
            case 0:
                switch (z) {
                    // Relative jumps and assorted ops
                    case 0:
                        switch (y) {
                            case 0:
                                NOP();
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 1:
                                LD_n16_n16(nn, registers.sp);
                                Cycles_AddM(5);
                                return (address + 3);
                                break;
                            case 2:
                                CPU_MissingInstruction("STOP");
                                return (address + 1);
                                break;
                            case 3:
                                Cycles_AddM(3);
                                return JR((Sint8) n);
                                break;
                            case 4 ... 7:
                                if (ConditionCodeCheck(Table_cc(y-4))) {
                                    Cycles_AddM(3);
                                    return JR((Sint8) n);
                                }
                                else {
                                    Cycles_AddM(2);
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
                                Cycles_AddM(3);
                                return (address + 3);
                                break;
                            case 1:
                                ADD_HL(Registers_ReadR16(Table_rp(p)));
                                Cycles_AddM(2);
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
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                    case 1:
                                        LD_n16_n8(registers.de, registers.a);
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                    case 2:
                                        LD_n16_n8(registers.hl, registers.a);
                                        registers.hl++;
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                    case 3:
                                        LD_n16_n8(registers.hl, registers.a);
                                        registers.hl--;
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                }
                            case 1:
                                switch (p) {
                                    case 0:
                                        LD_r8_n8(A, Memory_Read(registers.bc));
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                    case 1:
                                        LD_r8_n8(A, Memory_Read(registers.de));
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                    case 2:
                                        LD_r8_n8(A, Memory_Read(registers.hl));
                                        registers.hl++;
                                        Cycles_AddM(2);
                                        return (address + 1);
                                        break;
                                    case 3:
                                        LD_r8_n8(A, Memory_Read(registers.hl));
                                        registers.hl--;
                                        Cycles_AddM(2);
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
                                Cycles_AddM(2);
                                return (address + 1);
                                break;
                            case 1:
                                DEC_r16(Table_rp(p));
                                Cycles_AddM(2);
                                return (address + 1);
                                break;
                        }
                    break;

                    // 8-bit INC
                    case 4:
                        INC_r8(Table_r(y));
                        Cycles_AddM(1);
                        return (address + 1);
                    break;

                    // 8-bit DEC
                    case 5:
                        DEC_r8(Table_r(y));
                        Cycles_AddM(1);
                        return (address + 1);
                    break;

                    // 8-bit load immediate
                    case 6:
                        LD_r8_n8(Table_r(y), n);
                        Cycles_AddM(2);
                        return (address + 2);
                    break;

                    // Assorted operations on accumulator/flags
                    case 7:
                        switch (y) {
                            case 0:
                                CPU_MissingInstruction("RLCA");
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 1:
                                CPU_MissingInstruction("RRCA");
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 2:
                                CPU_MissingInstruction("RLA");
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 3:
                                RRA();
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 4:
                                CPU_MissingInstruction("DAA");
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 5:
                                CPU_MissingInstruction("CPL");
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 6:
                                CPU_MissingInstruction("SCF");
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                            case 7:
                                CPU_MissingInstruction("CCF");
                                Cycles_AddM(1);
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
                    LD_r8_n8(Table_r(y), Registers_ReadR8(Table_r(z)));

                    if ((Table_r(y) == HL8) || (Table_r(z) == HL8)) Cycles_AddM(2);
                    else Cycles_AddM(1);

                    return (address + 1);
                }
            break;

            case 2:
                // Operate on accumulator and register/memory location
                Table_alu_register(y, Table_r(z));

                if (Table_r(z) == HL8) Cycles_AddM(2);
                else Cycles_AddM(1);

                return (address + 1);
            break;

            case 3:
                switch (z) {
                    // Conditional return, mem-mapped register loads and stack operations
                    case 0:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) {
                                    Cycles_AddM(5);
                                    return RET();
                                }
                                else {
                                    Cycles_AddM(2);
                                    return (address + 1);
                                }
                                break;

                            case 4:
                                LD_n16_n8((0xFF00 + n), registers.a);
                                Cycles_AddM(3);
                                return (address + 2);
                                break;

                            case 5:
                                CPU_MissingInstruction("ADD SP, d");
                                Cycles_AddM(4);
                                return (address + 2);
                                break;

                            case 6:
                                LD_A_n16(0xFF00 + n);
                                Cycles_AddM(3);
                                return (address + 2);
                                break;

                            case 7:
                                CPU_MissingInstruction("LD HL, SP+ d");
                                Cycles_AddM(3);
                                return (address + 2);
                                break;
                        }
                        break;

                    // POP & various ops
                    case 1:
                        switch (q) {
                            case 0:
                                POP(Table_rp2(p));
                                Cycles_AddM(3);
                                return (address + 1);
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        Cycles_AddM(4);
                                        return RET();
                                        break;

                                    case 1:
                                        Cycles_AddM(4);
                                        return RETI();
                                        break;

                                    case 2:
                                        Cycles_AddM(1);
                                        return JP(registers.hl);
                                        break;

                                    case 3:
                                        Cycles_AddM(2);
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
                                    Cycles_AddM(4);
                                    return JP(nn);
                                }
                                else {
                                    Cycles_AddM(3);
                                    return (address + 3);
                                }
                                break;

                            case 4:
                                LD_n16_n8((0xFF00 + registers.c), registers.a);
                                Cycles_AddM(2);
                                return (address + 1);
                                break;

                            case 5:
                                LD_n16_n8(nn, registers.a);
                                Cycles_AddM(4);
                                return (address + 3);
                                break;

                            case 6:
                                LD_A_n16(0xFF00 + registers.c);
                                return (address + 1);
                                break;

                            case 7:
                                LD_A_n16(nn);
                                Cycles_AddM(2);
                                return (address + 3);
                                break;
                        }
                    break;

                    // Assorted operations
                    case 3:
                        switch (y) {
                            case 0:
                                Cycles_AddM(4);
                                return JP(nn);
                                break;

                            case 6:
                                DI();
                                Cycles_AddM(1);
                                return (address + 1);
                                break;

                            case 7:
                                EI();
                                Cycles_AddM(1);
                                return (address + 1);
                                break;
                        }
                    break;

                    // Conditional call
                    case 4:
                        switch (y) {
                            case 0 ... 3:
                                if (ConditionCodeCheck(Table_cc(y))) {
                                    Cycles_AddM(6);
                                    return CALL(nn);
                                }
                                else {
                                    Cycles_AddM(3);
                                    return (address + 3);
                                }
                                break;
                        }
                    break;

                    // PUSH & various ops
                    case 5:
                        switch (q) {
                            case 0:
                                PUSH(Registers_ReadR16(Table_rp2(p)));
                                Cycles_AddM(4);
                                return (address + 1);
                                break;

                            case 1:
                                switch (p) {
                                    case 0:
                                        Cycles_AddM(6);
                                        return CALL(nn);
                                        break;
                                }
                                break;
                        }
                    break;

                    // Operate on accumulator and immediate operand
                    case 6:
                        Table_alu(y, n);
                        Cycles_AddM(2);
                        return (address + 2);
                    break;

                    // Restart
                    case 7:
                        Cycles_AddM(4);
                        return RST(y*8);
                    break;
                }
            break;
        }
    }
    return (address + 1);
}

void CPU_Step() {
    #ifndef NDEBUG
    Memory_Log(memory, registers);
    #endif
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

    memory.memory[0xFF44] = 0x90;

    #ifndef NDEBUG
    Memory_LogInit();
    #endif
}

bool CPU_Halted() {return halted;}
bool CPU_Stopped() {return stopped;}
