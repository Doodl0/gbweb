#include <stddef.h>
#include <stdint.h>
#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <cpu.h>
#include <stdio.h>

// Based on stackoverflow.com/questions/69935188/open-a-file-in-emscripten-using-browser-file-selector-dialogue
// and github.com/Armchair-Software/emscripten-browser-file


#ifdef __EMSCRIPTEN__

EMSCRIPTEN_KEEPALIVE int LoadFile(const uint8_t *buffer, size_t size) {
  /// Load a file - this function is called from javascript when the file upload is activated
  SDL_Log("ROM %.16s loaded, buffer %p size %zu", buffer + 0x134, &buffer, size);

  // do whatever you need with the file contents
  SetMemory((unsigned char *) buffer, size);
  return 0;
}

#else

// TODO: Check file size
void LoadFile(char* filename) {
    FILE* file = fopen(filename, "rb");
    unsigned char buffer[0x8000];
    unsigned char *ptr = buffer;
    fread(ptr, sizeof(unsigned char), 0x8000, file);

    SDL_Log("ROM %.16s loaded from file, buffer %p size %zu" , ptr + 0x134, &ptr, 0x8000);

    SetMemory(ptr,0x8000);
    fclose(file);
}

#endif

void OpenFileDialog() {
    #ifdef __EMSCRIPTEN__
    EM_ASM(
      var file_selector = document.createElement('input');
      file_selector.setAttribute('type', 'file');
      file_selector.setAttribute('onchange','open_file(event)');
      file_selector.setAttribute('accept','.gb,.gbc'); // optional - limit accepted file types
      file_selector.click();
    );
    #else

    LoadFile("rom.gb");

    #endif
}
