#include <SDL.h>
#include <iostream>

#include "res/nes/nes.h"

int main(int argc, char** argv)
{
	nes emulator;

	emulator.Run();

	return 0;
}