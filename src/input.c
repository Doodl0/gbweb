#include "SDL3/SDL_events.h"
#include <SDL3/SDL.h>
#include <file.h>

#define UP SDL_SCANCODE_W
#define DOWN SDL_SCANCODE_S
#define LEFT SDL_SCANCODE_A
#define RIGHT SDL_SCANCODE_D
#define A_BUTTON SDL_SCANCODE_K
#define B_BUTTON SDL_SCANCODE_L
#define START SDL_SCANCODE_RETURN
#define SELECT SDL_SCANCODE_BACKSPACE
#define OPEN_ROM SDL_SCANCODE_TAB

void input(SDL_Event *event) {
    if (event->key.down) {
        if (event->key.scancode == UP) {
            SDL_Log("Up");
        }
        else if (event->key.scancode == DOWN) {
            SDL_Log("Down");
        }
        else if (event->key.scancode == LEFT) {
            SDL_Log("Left");
        }
        else if (event->key.scancode == RIGHT) {
            SDL_Log("Right");
        }
        else if (event->key.scancode == OPEN_ROM) {
            open_file_dialog();
        }
    }
}
