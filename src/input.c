#include "SDL3/SDL_events.h"
#include <SDL3/SDL.h>
#include <file.h>
#include <keypad.h>

#define UP SDL_SCANCODE_W
#define DOWN SDL_SCANCODE_S
#define LEFT SDL_SCANCODE_A
#define RIGHT SDL_SCANCODE_D
#define A_BUTTON SDL_SCANCODE_L
#define B_BUTTON SDL_SCANCODE_K
#define START SDL_SCANCODE_RETURN
#define SELECT SDL_SCANCODE_BACKSPACE
#define OPEN_ROM SDL_SCANCODE_TAB

void Input(SDL_Event *event) {
    // Key down events
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.scancode == UP) Keypad_KeyDown(KEY_UP);
        else if (event->key.scancode == DOWN) Keypad_KeyDown(KEY_DOWN);
        else if (event->key.scancode == LEFT) Keypad_KeyDown(KEY_LEFT);
        else if (event->key.scancode == RIGHT) Keypad_KeyDown(KEY_RIGHT);
        else if (event->key.scancode == A_BUTTON) Keypad_KeyDown(KEY_A);
        else if (event->key.scancode == B_BUTTON) Keypad_KeyDown(KEY_B);
        else if (event->key.scancode == SELECT) Keypad_KeyDown(KEY_SELECT);
        else if (event->key.scancode == START) Keypad_KeyDown(KEY_START);
        else if (event->key.scancode == OPEN_ROM) File_OpenDialog();
    }
    // Key up events
    else if (event->type == SDL_EVENT_KEY_UP) {
        if (event->key.scancode == UP) Keypad_KeyUp(KEY_UP);
        else if (event->key.scancode == DOWN) Keypad_KeyUp(KEY_DOWN);
        else if (event->key.scancode == LEFT) Keypad_KeyUp(KEY_LEFT);
        else if (event->key.scancode == RIGHT) Keypad_KeyUp(KEY_RIGHT);
        else if (event->key.scancode == A_BUTTON) Keypad_KeyUp(KEY_A);
        else if (event->key.scancode == B_BUTTON) Keypad_KeyUp(KEY_B);
        else if (event->key.scancode == SELECT) Keypad_KeyUp(KEY_SELECT);
        else if (event->key.scancode == START) Keypad_KeyUp(KEY_START);
    }
}
