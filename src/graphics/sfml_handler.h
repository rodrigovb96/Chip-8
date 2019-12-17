#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>


using Key = sf::Keyboard::Key;

template <class Emulator>
class Sfml_handler
{
    sf::RenderWindow window;

    sf::Uint8 pixels[2048];

    Key keymap[16] =
    {
        Key::X,
        Key::Num1,
        Key::Num2,
        Key::Num3,
        Key::Q,
        Key::W,
        Key::E,
        Key::A,
        Key::S,
        Key::D,
        Key::Z,
        Key::C,
        Key::Num4,
        Key::R,
        Key::F,
        Key::V
    };

    Emulator emu;

public:
    Sfml_handler() : window(sf::VideoMode(1024, 512), "Chip-8 Emulator!")
    {
    }

    Emulator* get_emu() { return &emu; }

    void loop()
    {

        while(window.isOpen())
        {
            emu.emulate_cycle();

            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                if(event.type == sf::Event::KeyPressed)
                {
                    for(int i = 0; i < 16; i++)
                    {
                        if(event.key.code == keymap[i])
                            emu.set_key_pressed(i);

                    }
                }

                if(event.type == sf::Event::KeyReleased) 
                {
                    for(int i = 0; i < 16; i++)
                    {
                        if(event.key.code == keymap[i])
                            emu.release_key(i);
                    }

                }

            }

            if(emu.need_to_draw())
            {
                for(int i = 0; i < 2048; i++)
                {
                    auto pixel = emu.pixel_in(i);
                    pixels[i] = pixel; 
                }

                window.clear();
                draw_display(); 
                window.display();

                emu.set_draw_flag();

            }


            //std::this_thread::sleep_for(std::chrono::microseconds(1200));
        }
    }

    void draw_display()
    {
        //define what pixel size::::since resolution is constant, variables won't be used
        sf::RectangleShape rectangle(sf::Vector2f(13,13));
        for(int y=0; y < 32; ++y){
            for(int x=0; x < 64; ++x){
                unsigned char gfx = pixels[x+(64*y)];
                if(gfx > 0){
                    rectangle.setPosition((float)(x*13), (float)(y*13));
                    window.draw(rectangle);
                }
            }
        }
    }



};
