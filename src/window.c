#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <wchar.h>
#include <window.h>

#define STARTING_RESOLUTION_SCALE 5

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT 1
#endif

static SDL_Window *window = NULL;
static SDL_Renderer *renderer;

static SDL_Surface* framebuffer;

bool Window_Init() {

    // Create window and renderer
    if (!SDL_CreateWindowAndRenderer("GBWeb", 160 * STARTING_RESOLUTION_SCALE, 144 * STARTING_RESOLUTION_SCALE, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &renderer)) {
        return false;
    }
    // Set fixed aspect ratio and minimum size
    SDL_SetWindowAspectRatio(window, 1.1111111, 1.1111111);
    SDL_SetWindowMinimumSize(window, 160, 144);
    // Create framebuffer
    framebuffer = SDL_CreateSurface(160, 144, SDL_PIXELFORMAT_RGBA8888);
    return true;
}

void Window_NoROM() {

    char* message[] = {"No ROM loaded",
        "Press TAB to load a ROM",
        "",
        "Controls:",
        "WASD for D-Pad",
        "A button = L key",
        "B button = K Key",
        "Select = Backspace",
        "Start = Enter/Return"
    };

    // Get text scale based on window size
    int window_x, window_y;
    SDL_GetWindowSize(window, &window_x, &window_x);
    const float scale = window_x / 160.0f;
    SDL_SetRenderScale(renderer, scale, scale);

    // First message should be a bit above centre
    int w = 0, h = 0;
    float x, y;

    // Center the message and scale it up
    SDL_GetCurrentRenderOutputSize(renderer, &w, &h);

    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2 - 40;

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Loop over lines and draw
    for(int i = 0; i < 9; i++) {
        // Centre x coordinate
        x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message[i])) / 2;

        // Draw message line
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, x, y + 10 * i, message[i]);
    }

    SDL_RenderPresent(renderer);
}

void Window_DrawPixel(int pixelOffset, pixelColour colour) {
    int y = pixelOffset / (int)160;
    int x = pixelOffset % 160;

    SDL_WriteSurfacePixel(framebuffer, x, y, colour.r, colour.g, colour.b, 255);
}

void Window_Blit() {
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, framebuffer);
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_PIXELART);
    SDL_RenderTexture(renderer, tex, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(tex);
}
