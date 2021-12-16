#ifndef _NES_H_
#define _NES_H_

#include <SDL.h>

// Include components
#include "../sfot/sfot.h"
#include "nescart.h"
#include "nesppu.h"

// The all encompassing NES emulator class
struct nes
{
private: // OS Window elements
	SDL_Window* m_Window{ nullptr };
	SDL_Renderer* m_Context{ nullptr };

private: // Emulator variables
	u32 m_CPUCycles{ 0 }, m_CPUStep{ 0 };
	bool m_PPUNMI{ false };
	bool m_PPUFrame{ false };

public: // NES emu components
	sfot m_Processor;
	nesbus m_Bus;
	nescart m_Cartridge;
	nesppu m_PictureProcessingUnit;

private: // Internal methods
	/// @brief Runs all components to be cycle-accurate
	/// @return The time it took
	double Clock();

	/// @brief Used for Tick function for sfot
	void CPUTick();

	/// @brief Used for NMI function for ppu to generate NMIs
	void PPUNMIRequest() { m_PPUNMI = true; };

public: // Public methods
	/// @brief The main emulation loop
	void Run();

public: // Instantiation
	nes(SDL_Window* _window, SDL_Renderer* _context) : m_Window{ _window }, m_Context{ _context } 
	{ 
		m_Bus.m_ROM = &m_Cartridge; 
		m_Bus.m_PPUREG = &m_PictureProcessingUnit.r_REGISTERS;
		m_Processor.Tick = std::bind(&nes::CPUTick, this); 
		m_PictureProcessingUnit.GenNMI = std::bind(&nes::PPUNMIRequest, this);
	}
	nes(const nes &) = delete;
};

#endif // !_NES_H_
