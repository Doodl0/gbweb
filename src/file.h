#include <stdint.h>

void OpenFileDialog();

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE int LoadFile(char* filename, char *buffer, size_t size, void *callback_data);

#endif
