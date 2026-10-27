#include <keypad.h>
#include <SDL3/SDL_stdinc.h>
#include <interrupts.h>

Uint8 rows[2] = {0x0F, 0x0F};
Uint8 column = 0;

void Keypad_Reset() {
    rows[0] = 0x0F;
    rows[1] = 0x0F;
    column = 0;
}

Uint8 Keypad_Read() {
    switch(column) {
        case 0x10: return rows[0];
        case 0x20: return rows[1];
        case 0x30: return 0x0F;
        default: return 0x0F;
    }
}

void Keypad_Write(Uint8 value) {
    column = value & 0x30;
}

void Keypad_KeyDown(Keypad_Keys key) {
    Interrupts_FlagSet(INTERRUPTS_JOYPADPRESS, 1);
    switch(key) {
        case KEY_UP: rows[1] &= 0xB; break;
        case KEY_DOWN: rows[1] &= 0x7; break;
        case KEY_LEFT: rows[1] &= 0xD; break;
        case KEY_RIGHT: rows[1] &= 0xE; break;
        case KEY_A: rows[0] &= 0xE; break;
        case KEY_B: rows[0] &= 0xD; break;
        case KEY_SELECT: rows[0] &= 0xB; break;
        case KEY_START: rows[0] &= 0x7; break;
    }
}

void Keypad_KeyUp(Keypad_Keys key) {
    switch(key) {
        case KEY_UP: rows[1] |= 0x4; break;
        case KEY_DOWN: rows[1] |= 0x8; break;
        case KEY_LEFT: rows[1] |= 0x2; break;
        case KEY_RIGHT: rows[1] |= 0x1; break;
        case KEY_A: rows[0] |= 0x1; break;
        case KEY_B: rows[0] |= 0x2; break;
        case KEY_SELECT: rows[0] |= 0x4; break;
        case KEY_START: rows[0] |= 0x8; break;
    }
}
