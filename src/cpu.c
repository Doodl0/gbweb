struct Instruction {
    char* dissassembly;
    unsigned char operandLength;
    void* execute;
} typedef instruction;

enum ArithmeticTarget {
    A, B, C, D, E, H, L
} typedef arithmetic_target;

/*
const struct instruction instructions[256] {

}
*/

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
