
#define WASM

#include "src/graphics/sdl2_handler.h"
#include <emscripten.h>
#include "src/chip8.cpp"

SDL2_handler<chip8::Core> h;


void wasm_loop()
{
    h.loop();
}

int main(int argc, char ** argv )
{
    h.get_emu()->load_ROM("chip8Pic.ch8");

    emscripten_set_main_loop(&wasm_loop,-1,1);

}
