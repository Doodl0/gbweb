#include <stdint.h>

void File_OpenDialog();

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE int LoadFile(char* filename, char *buffer, size_t size, void *callback_data);

#else

void LoadFile(char* filename);

#endif
