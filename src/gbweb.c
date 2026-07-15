#include <SDL/SDL_stdinc.h>
#include <SDL/SDL_video.h>
#include <stdio.h>
#include <SDL/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Emulator Settings
#define RESOLUTION_SCALE 4

#define UP SDL_SCANCODE_W
#define DOWN SDL_SCANCODE_S
#define LEFT SDL_SCANCODE_A
#define RIGHT SDL_SCANCODE_D
#define A_BUTTON SDL_SCANCODE_K
#define B_BUTTON SDL_SCANCODE_L
#define START SDL_SCANCODE_RETURN
#define SELECT SDL_SCANCODE_BACKSPACE

int running = 1;

static void init(void) {
    printf("Hello, world!\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        running = false;
    }

    SDL_Window *window = SDL_CreateWindow("GBWeb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * RESOLUTION_SCALE, 144 * RESOLUTION_SCALE, SDL_WINDOW_INPUT_FOCUS);
}

static void deinit() {
    SDL_Quit();
}

void draw_frame() {

}

void input() {
    const Uint8 *key_states = SDL_GetKeyboardState(NULL);
    if (key_states[UP]) { printf("UP BUTTON PRESSED \n"); }
    if (key_states[DOWN]) { printf("DOWN BUTTON PRESSED \n"); }
    if (key_states[LEFT]) { printf("LEFT BUTTON PRESSED \n"); }
    if (key_states[RIGHT]) { printf("RIGHT BUTTON PRESSED \n"); }
    if (key_states[A_BUTTON]) { printf("A BUTTON PRESSED \n"); }
    if (key_states[B_BUTTON]) { printf("B BUTTON PRESSED \n"); }
    if (key_states[START]) { printf("START BUTTON PRESSED \n"); }
    if (key_states[SELECT]) { printf("SELECT BUTTON PRESSED \n"); }
}

static void mainloop(void) {
    if (!running) {
        deinit();
        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();  /* this should "kill" the app. */
        #else
        exit(0);
        #endif
    }

    input();
    draw_frame();
}

int main(int argc, char** argv) {
    init();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
    #else
    while (1) { mainloop(); }
    #endif
}
