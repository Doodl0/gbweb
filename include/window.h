#include <stdbool.h>
#include <SDL3/SDL_stdinc.h>

typedef struct colour {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} pixelColour;

bool Window_Init();
void Window_NoROM();
void Window_DrawPixel(int pixelOffset, pixelColour colour);
void Window_Blit();
