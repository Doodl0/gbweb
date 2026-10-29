![GBWeb Banner](https://github.com/Doodl0/gbweb/blob/main/html/assets/gbweb-banner.png?raw=true)
# GBWeb
### A Gameboy emulator, written in C, designed to run entirely in your web browser (using WebAssembly / Emscripten).
[Try it now!](https://doodl0.github.io/gbweb/) (still quite inaccurate, will not run all games. Both Tetris and Super Mario Land should run, but I cannot guarantee any other games at the moment)

## Current Features
- Accurate CPU emulation that passes all of [blargg's cpu_instrs test ROMs](https://github.com/retrio/gb-test-roms/tree/master/cpu_instrs)
- MBC1 cartridge mapper support (Tested in Super Mario Land and blargg's Multi-ROM test)
- Semi-accurate PPU; will probably get improved in the future
- Runs within a web browser locally on your device

## Controls
- D-Pad -> WASD keys
- A button -> L key
- B button -> K key
- Select button -> Backspace key
- Start button -> Enter key
These are also shown before loading a ROM

## Running / Building
- You can try it right now at [doodl0.github.io/gbweb/](https://doodl0.github.io/gbweb/)!
- **If you don't have any ROMs to test, you can watch the demo video on the page or you can try the provided tic-tac-toe homebrew ROM, which can be downloaded [here](https://doodl0.github.io/gbweb/assets/GB-TicTacToe.gb) or on the webpage as a hyperlink.**
- Alternatively, you can build a desktop version using CMake. There are provided build scripts in the repo, although these are rather basic and will only work on Linux. If you want to run it on desktop, you must rename your ROM file to `rom.gb` and place it in the same directory as the executable. (Sorry about this, I made this with a web-first design plan and didn't have time to add a local file dialog library)
- If you want to compile it yourself, make sure to git clone with `--recurse-submodules`, so that SDL3 is also downloaded.
- Additionally, if you want to build a web version yourself, you need to install the Emscripten SDK. You can read the instructions for that [here](https://emscripten.org/docs/getting_started/downloads.html).

## How it works
- This project is a pretty basic implementation of a standard Gameboy emulator. On a simple level, a Gameboy ROM file is read, and each of the instructions are processed one by one, like the Gameboy would do. There is also emulated memory, where the emulated CPU can read and write to, and an emulated PPU (Picture processing unit), which tries to recreate the visual output of the Gameboy. These all attempt to run at the same speed as the original Gameboy, with the CPU being 4.19 MHz and the PPU running at 59.7 frames per second.
- In terms of tooling, the project only used SDL3, which provides cross-platform input events, windowing and basic drawing to the screen. I also used Emscripten, a tool / compiler that helps to compile C code to WebAssembly, which already had support for SDL3, making it easy to implement.

## References / Resources / Acknowledgements
- [SDL Wiki](https://wiki.libsdl.org/)
- File loading in Emscripten - [This Stack Overflow thread](https://stackoverflow.com/questions/69935188/open-a-file-in-emscripten-using-browser-file-selector-dialogue) and [emscripten-browser-file](https://github.com/Armchair-Software/emscripten-browser-file)
- [The gbdev.io emulator development resources list](https://gbdev.io/resources.html#emulator-development)
- [The Gameboy Pan Docs](https://gbdev.io/pandocs/)
- [Development blog of Cinoop](https://cturt.github.io/cinoop.html)
- [DMG-01 book / guide](https://rylev.github.io/DMG-01/)
- [Decoding Z80 opcodes](https://archive.gbdev.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html)
- [RGBDS instruction list](https://rgbds.gbdev.io/docs/v1.0.4/gbz80.7)
- [Gameboy Doctor](https://github.com/robert/gameboy-doctor)
- [Post I made asking for help on r/EmuDev](https://www.reddit.com/r/EmuDev/comments/1wtn4j7/gb_emulation_stuck_on_ld_in_blarggs_cpu_instrcs/)
- [Gameboy emulation in Javascript / js-GB tutorial series](https://imrannazar.com/series/gameboy-emulation-in-javascript)
- [Demystifying the GameBoy/SM83’s DAA Instruction](https://blog.ollien.com/posts/gb-daa/)
- [Blargg's cpu_instrs test ROMS](https://github.com/retrio/gb-test-roms/tree/master/cpu_instrs)

## Screenshots
![Screenshot Tetris Title](https://github.com/Doodl0/gbweb/blob/main/html/assets/ScreenshotTetrisTitle.png?raw=true)

![Screenshot Tetris Gameplay](https://github.com/Doodl0/gbweb/blob/main/html/assets/ScreenshotTetrisGameplay.png?raw=true)

![Screenshot Super Mario Land Title](https://github.com/Doodl0/gbweb/blob/main/html/assets/ScreenshotSuperMarioLandTitle.png?raw=true)

![Screenshot Super Mario Land Gameplay](https://github.com/Doodl0/gbweb/blob/main/html/assets/ScreenshotSuperMarioLandGameplay.png?raw=true)

![Screenshot Super Mario Land Bonus Game](https://github.com/Doodl0/gbweb/blob/main/html/assets/ScreenshotSuperMarioLandBonusGame.png?raw=true)
