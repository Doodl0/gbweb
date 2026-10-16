#include "SDL3/SDL_render.h"
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <wchar.h>
#include <window.h>

#define RESOLUTION_SCALE 4

static SDL_Window *window = NULL;
static SDL_Renderer *renderer;

static SDL_Surface* framebuffer;

bool Window_Init() {
    // Create window and renderer
    if (!SDL_CreateWindowAndRenderer("GBWeb", 160 * RESOLUTION_SCALE, 144 * RESOLUTION_SCALE, 0, &window, &renderer)) {
        return false;
    }
    framebuffer = SDL_CreateSurface(160, 144, SDL_PIXELFORMAT_RGBA8888);
    return true;
}

void Window_DrawPixel(int pixelOffset, pixelColour colour) {
    int y = pixelOffset / 160;
    int x = pixelOffset % 160;
    SDL_WriteSurfacePixel(framebuffer, x, y, colour.r, colour.g, colour.b, 255);
}

void Window_Blit() {
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, framebuffer);
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(renderer, tex, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(tex);
}
