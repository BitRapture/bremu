#include "nes.h"

#include <iostream>

void nes::CPUTick()
{
	for (u32 i = 0; i < 3; ++i) { m_PPU.EmulateCycle(m_Bus); }
}

void nes::PPUFrame()
{
	// Transfer ppu pixel array to sdl texture
	SDL_RenderClear(m_Context);
	u32* pixels = nullptr; int pitch;
	SDL_LockTexture(m_PPUScreen, 0, (void**)&pixels, &pitch);
	for (u32 i = 0; i < 61440; ++i) // 256 * 240 = 61440 (screen resolution)
	{
		pixels[i] = (m_PPU.m_Screen[i] << 8) | 0xFF;
	}
	SDL_UnlockTexture(m_PPUScreen);
	SDL_RenderCopy(m_Context, m_PPUScreen, 0, 0);
	SDL_RenderPresent(m_Context);
}

void nes::Clock()
{
	// nestest ->    std::cout << m_CPU.GetDebugInfo() << " CYC:" << std::dec << e_Cycles << std::endl; 

	// Synchronize to cpu cycles (1 cpu cycle = 3 ppu cycles)
	if (m_PPU.GetGenNMI())
	{ m_PPU.ClearGenNMI(); m_CPU.NMI(m_Bus); }
	else { m_CPU.EmulateStep(m_Bus); }

}

void nes::Run()
{
	// Load game into cartridge port
	m_Cartridge.LoadCartridge("./carts/nestest.nes");
	if (!m_Cartridge.IsLoaded()) { return; }
	m_PPU.Reset();
	m_CPU.Reset(m_Bus);

	bool runtime = true;
	while (runtime)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				runtime = false;
				break;
			}
		}

		Clock();
	}
}

nes::nes(SDL_Window* _window, SDL_Renderer* _context)
	:
	m_Window{ _window }, m_Context{ _context }
{
	// Connect components to eachother
	m_Bus.m_Cart = &m_Cartridge;
	m_Bus.m_PPUReg = &m_PPU.m_Reg;
	// Connect function pointers
	m_CPU.Tick = std::bind(&nes::CPUTick, this);
	m_PPU.CreateFrame = std::bind(&nes::PPUFrame, this);

	// Create SDL variables
	m_PPUScreen = SDL_CreateTexture(m_Context, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
	SDL_SetTextureBlendMode(m_PPUScreen, SDL_BLENDMODE_NONE);
}

nes::~nes()
{
	SDL_DestroyTexture(m_PPUScreen);
	SDL_Quit();
}