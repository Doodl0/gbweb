#include <SDL3/SDL_stdinc.h>
#include <stddef.h>
#include <stdio.h>
#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <memory.h>
#include <cpu.h>
#include <main.h>

// Based on stackoverflow.com/questions/69935188/open-a-file-in-emscripten-using-browser-file-selector-dialogue
// and github.com/Armchair-Software/emscripten-browser-file

#ifdef __EMSCRIPTEN__

// Javascript code to read file
EM_JS(void, open_file, (int e), {
          const file_reader = new FileReader();
          file_reader.onload = (event) => {
            const uint8Arr = new Uint8Array(event.target.result);
            const num_bytes = uint8Arr.length * uint8Arr.BYTES_PER_ELEMENT;
            const data_ptr = Module._malloc(num_bytes);
            const data_on_heap = new Uint8Array(Module.HEAPU8.buffer, data_ptr, num_bytes);
            data_on_heap.set(uint8Arr);
            const res = Module.ccall('File_Load', 'number', ['number', 'number'], [data_on_heap.byteOffset, uint8Arr.length]);
            Module._free(data_ptr);
          };
          file_reader.readAsArrayBuffer(e.target.files[0]);
        };
);

EMSCRIPTEN_KEEPALIVE int File_Load(const Uint8 *buffer, size_t size) {
  /// Load a file - this function is called from javascript when the file upload is activated
  SDL_Log("ROM %.16s loaded, buffer %p size %zu", buffer + 0x134, &buffer, size);

  // do whatever you need with the file contents
  Memory_Set((Uint8 *) buffer, size);

  CPU_Init();
  Main_SetROMLoaded();

  return 0;
}

#else

void File_Load(char* filename) {
    FILE* file = fopen(filename, "rb");
    // ROM has a max of 256 banks, which is (256 * 16KiB) which is 4MiB
    Uint8 buffer[0x400000];
    Uint8 *ptr = buffer;
    fread(ptr, sizeof(Uint8), 0x400000, file);

    SDL_Log("ROM %.16s loaded from file, buffer %p" , ptr + 0x134, &ptr);

    Memory_Set(ptr, 0x400000);
    fclose(file);

    CPU_Init();
    Main_SetROMLoaded();
}

#endif

void File_OpenDialog() {
    #ifdef __EMSCRIPTEN__

    EM_ASM(
      var file_selector = document.createElement('input');
      file_selector.setAttribute('type', 'file');
      file_selector.setAttribute('onchange','open_file(event)');
      file_selector.setAttribute('accept','.gb,.gbc'); // optional - limit accepted file types
      file_selector.click();
    );
    #else

    File_Load("smb.gb");

    #endif
}
