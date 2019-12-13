#include "src/graphics/sfml_handler.h"
#include <iostream>
#include "src/chip8.cpp"


int main(int argc, char ** argv )
{

    Sfml_handler<chip8::Core> h;

    h.get_emu()->load_ROM(argv[1]);


    h.loop();
    

}
