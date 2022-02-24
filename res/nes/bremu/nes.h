#ifndef _NES_H_
#define _NES_H_

#include <SDL.h>
#include "../../defs.h"

#include "../../sfot/sfot.h"
#include "../nesppu.h"
#include "../nescart.h"
#include "../nesbus.h"
#include "bremucont.h"

// BREMU exclusive
// The all encompassing nes structure
struct nes
{
private: // SDL variables
	SDL_Window* m_Window{ nullptr };
	SDL_Renderer* m_Context{ nullptr };

	// PPU Display
	SDL_Texture* m_PPUScreen;

private: // Emulation variables
	u32 e_Cycles{ 0 };
	u32 e_CycleStart{ 0 };

private: // Components
	sfot m_CPU;
	nesppu m_PPU;
	nescart m_Cartridge;
	nesbus m_Bus;
	bremucont m_Player1, m_Player2;

private: // Internal methods
	void CPUTick();
	void PPUFrame();

	void Clock();

public: // Public methods
	void Run();

public: // Instantiation
	nes(SDL_Window* _window, SDL_Renderer* _context);
	~nes();
};

#endif // !_NES_H_
