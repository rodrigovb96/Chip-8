#include <SDL2/SDL.h>
#include <chrono>
#include "constants.h"
#include <cstdlib>
#ifdef DEBUG
#include <iostream>
#endif 
#include <thread>


namespace cts = constants;

template <class Emulator>
class SDL2_handler
{

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;

	uint32_t pixels[2048];

	uint32_t keymap[16] =
	{
		SDLK_x,
		SDLK_1,
		SDLK_2,
		SDLK_3,
		SDLK_q,
		SDLK_w,
		SDLK_e,
		SDLK_a,
		SDLK_s,
		SDLK_d,
		SDLK_z,
		SDLK_c,
		SDLK_4,
		SDLK_r,
		SDLK_f,
		SDLK_v
	};

	bool running{true};

	Emulator emu;

public:
	SDL2_handler()
	{
		SDL_Init(SDL_INIT_VIDEO);
		SDL_CreateWindowAndRenderer(cts::screen::width,cts::screen::height, 0, &window, &renderer);

	}

	Emulator* get_emu() { return &emu; }



	void loop()
	{

		int32_t frame_speed;
		int32_t start_tick;

#ifndef WASM
		while(running)
#endif
		{
			emu.emulate_cycle();
			//start_tick = SDL_GetTicks();
			

			


			if( emu.need_to_draw() )
			{
				//std::cerr << "emu.need_to_draw()...\n";

				for(int i = 0; i < 2048 ; i++)
				{
					auto pixel = emu.pixel_in(i);
					pixels[i] = pixel; 
				}

				draw_display();
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderClear(renderer);


				emu.no_need_to_draw();
			}

			/*frame_speed = SDL_GetTicks() - start_tick;
			if (frame_speed < (1000/60) ) {
			   SDL_Delay((1000/60)- frame_speed);
			}*/
			SDL_Event event;
			while(SDL_PollEvent(&event))
			{
				switch(event.type) 
				{
					case SDL_QUIT:
					{
						running = false;
					}
					case SDL_KEYDOWN: 
					{
						if( event.key.keysym.sym == SDLK_ESCAPE) 
						{
							running = false;
							break;
						}
							
						for(int i{0}; i < 16; i++)
						{
							if(event.key.keysym.sym == keymap[i])
							{
								emu.set_key_pressed(i);
								break;
							}

						}

						break;
					}
					case SDL_KEYUP:
					{
						for(int i{0}; i < 16; i++)
						{
							if(event.key.keysym.sym == keymap[i])
							{
								emu.release_key(i);
							}

						}
					}



				}


			}
#ifndef WASM
		    std::this_thread::sleep_for(std::chrono::microseconds(1200));
#endif
		}




	}

	void draw_display()
	{
		SDL_RenderClear(renderer);
		SDL_Rect rectangle;
		rectangle.h = 13;
		rectangle.w = 13;
		for(int y=0; y < 32; ++y){
			for(int x=0; x < 64; ++x){
				unsigned char gfx = pixels[x+(64*y)];
				if(gfx > 0){
					rectangle.x = (x*13); 
					rectangle.y = (y*13);
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					SDL_RenderFillRect(renderer, &rectangle );
				}
			}
		}
		SDL_RenderPresent(renderer);


	}

};
