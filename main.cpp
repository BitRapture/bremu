#include <SDL.h>
#include <iostream>

#include "res/nes/bremu/nes.h"

int main(int argc, char** argv)
{
	// Boilerplate SDL2 initialization
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) == -1) { return -1; }
	SDL_Window* window = SDL_CreateWindow("BREMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 480, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* context = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	nes emulator(window, context);
	emulator.Run();

	SDL_DestroyRenderer(context);
	SDL_DestroyWindow(window);
	return 0;
}