#include <stdint.h>

void open_file_dialog();

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE int load_file(char* filename, char *buffer, size_t size, void *callback_data);

#endif
