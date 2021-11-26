#ifndef _NES_H_
#define _NES_H_

// Include components
#include "../sfot/sfot.h"
#include "nescart.h"

// The all encompassing NES emulator class
struct nes
{
public: // NES emu components
	sfot m_Processor;
	nesbus m_Bus;
	nescart m_Cartridge;

public: // Public methods
	/// @brief The main emulation loop
	void Run();

public: // Instantiation
	nes() { m_Bus.m_ROM = &m_Cartridge; }

};

#endif // !_NES_H_
