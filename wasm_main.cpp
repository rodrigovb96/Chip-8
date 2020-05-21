
#define WASM

#include "src/graphics/sdl2_handler.h"
#include <emscripten.h>
#include <memory>
#include "src/chip8.cpp"

std::unique_ptr<SDL2_handler<chip8::Core>> h = std::make_unique< SDL2_handler<chip8::Core> >();


void wasm_loop()
{
    h->loop();
}

int main(int argc, char ** argv )
{
    h->get_emu()->load_ROM("chip8Pic.ch8");

    emscripten_set_main_loop(&wasm_loop,60,1);

}
