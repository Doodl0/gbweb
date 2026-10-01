#include <stdint.h>

void File_OpenDialog();

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE int File_Load(char* filename, char *buffer, size_t size, void *callback_data);

#else

void File_Load(char* filename);

#endif
