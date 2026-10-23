#include "SDL3/SDL_timer.h"
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_main.h>
#include <input.h>
#include <file.h>
#include <cycles.h>
#include <ppu.h>
#include <cpu.h>
#include <window.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT 1
#endif

bool ROMLoaded = false;

// Set a constant frametime for an FPS limit
#define LIMIT_FPS 1
const Uint64 FRAMETIME = (1 / 59.7) * 1000000000;
Uint64 frameStart = 1000000;

void Main_SetROMLoaded() {ROMLoaded = true;}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Create window and renderer
    if (!Window_Init()) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    Input(event);
    return SDL_APP_CONTINUE;
}

// Per frame iteration
SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Only render 59.7 times per second
    // Each frame runs the cpu for the amount of time that the CPU would use during a frame, and then renders
    // This is a lot more efficient that using the CPU as many times per second and then rendering at 60FPS
    // because of SDL bottlenecks, meaning this function can't run that fast.

    if ((SDL_GetTicksNS() - frameStart) > FRAMETIME || !LIMIT_FPS)  {
        frameStart = SDL_GetTicksNS();
        if (ROMLoaded) {
            Cycles_Step();
        }
        else {
            Window_NoROM();
        }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{

}
