#include <stdint.h>
#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Based on stackoverflow.com/questions/69935188/open-a-file-in-emscripten-using-browser-file-selector-dialogue
// and github.com/Armchair-Software/emscripten-browser-file

void open_file_dialog() {
    #ifdef __EMSCRIPTEN__
    EM_ASM(
      var file_selector = document.createElement('input');
      file_selector.setAttribute('type', 'file');
      file_selector.setAttribute('onchange','open_file(event)');
      file_selector.setAttribute('accept','.gb,.gbc'); // optional - limit accepted file types
      file_selector.click();
    );
    #endif
}

#ifdef __EMSCRIPTEN__

EMSCRIPTEN_KEEPALIVE int load_file(uint8_t *buffer, size_t size) {
  /// Load a file - this function is called from javascript when the file upload is activated
  SDL_Log("load_file triggered, buffer %p size %d", &buffer, size);

  // do whatever you need with the file contents

  return 1;
}
#endif
