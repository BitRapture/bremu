#include <iostream>

#include "res/sfot/sfot.h"
#define op (u8)sfotops::

int main()
{
	nesbus bus;
	sfot processor;
	
	// Delete full cart
	delete bus.m_ROM;
	// Set to test rom
	bus.m_ROM = new u8[]
	{
		op LDA_IMM,			// Load accumulator with value 0x45 (69)
		0x45,
		op TAX,				// Transfer accumulator value to X register
		op STX_ABS,			// Store value of X register to the memory location 0x0000
		0x00, 0x00
	};

	// Test bus accessing
	bus[0x0000] = 27;  // Set RAM(0x0000) to 27

	// Set to ROM location
	processor.SetProgramCounter(0x4020);
	// Step through 3 instructions
	processor.EmulateStep(bus); // LDA_IMM
	processor.EmulateStep(bus);	// TAX
	processor.EmulateStep(bus);	// STX <- comment this one out and RAM(0x0000) equals 27

	std::cout << (u32)bus[0x1000] << ", " << (u32)bus.m_RAM[0x0000] << std::endl;	// If failed it'll print 27, if passed it'll print 69

	return 0;
}