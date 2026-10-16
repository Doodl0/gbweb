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
#endif



// Set a constant frametime for an FPS limit
const Uint64 FPS = (1 / 59.7) * 1000000000;
Uint64 frameStart = 1000000;



SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Create window and renderer
    if (!Window_Init()) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    File_OpenDialog();
    CPU_Init();

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
    // Only render 5.97 times per second
    // Each frame runs the cpu for the amount of time that the CPU would use during a frame, and then renders
    // This is a lot more efficient that using the CPU as many times per second and then rendering at 60FPS
    // because of SDL bottlenecks, meaning this function can't run that fast.

    if (SDL_GetTicksNS() - frameStart > FPS) {
        Cycles_Step();
        Window_Blit();
        frameStart = SDL_GetTicksNS();
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{

}
