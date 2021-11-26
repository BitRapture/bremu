#include <iostream>
#include <SDL.h>

#include "res/sfot/sfot.h"
#include "res/nes/nescart.h"
#define op (u8)sfotops::

int main(int argc, char** argv)
{
	nescart cart;
	nesbus bus(cart);
	sfot processor;
	
	cart.LoadCartridge("./carts/mario.nes");

	std::cout << "done" << std::endl;

	return 0;
}