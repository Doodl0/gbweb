#include <SDL3/SDL_stdinc.h>

typedef enum Keypad_Keys {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_A,
    KEY_B,
    KEY_SELECT,
    KEY_START
} Keypad_Keys;

Uint8 Keypad_Read();
void Keypad_Write(Uint8 value);
void Keypad_KeyDown(Keypad_Keys key);
void Keypad_KeyUp(Keypad_Keys key);
