#ifdef SFML
#include "src/graphics/sfml_handler.h"
#else
#include "src/graphics/sdl2_handler.h"
#endif 
#include <iostream>
#include "src/chip8.cpp"


int main(int argc, char ** argv )
{

#ifdef SFML
    Sfml_handler<chip8::Core> h;
#else 
    SDL2_handler<chip8::Core> h;
#endif

    h.get_emu()->load_ROM(argv[1]);


    h.loop();
    

}
