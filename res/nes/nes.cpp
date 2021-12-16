#include "nes.h"
#include <iostream>
#include <iomanip>

#define EMULATE() std::cout << std::hex << m_Processor.GetProgramCounter() << ": " << (u32)m_Bus.CPUReadMapping(m_Processor.GetProgramCounter()) <<" " << (u32)m_Bus.CPUReadMapping(m_Processor.GetProgramCounter() + 1) <<" " << (u32)m_Bus.CPUReadMapping(m_Processor.GetProgramCounter() + 2) << std::endl;

double nes::Clock()
{
	if (m_PPUNMI) { m_Processor.NMI(m_Bus); m_PPUNMI = false; }
	else { m_Processor.EmulateStep(m_Bus); }
	//m_PPUFrame = m_PictureProcessingUnit.EmulateCycle(m_Bus);
	//if (++m_CPUStep >= 3)
	//{
	//	if (m_CPUCycles == 0) 
	//	{ 
	//		if (m_PPUNMI) { m_Processor.NMI(m_Bus); m_PPUNMI = false; }
	//		else { m_Processor.EmulateStep(m_Bus); }
	//	}
	//	else { --m_CPUCycles; }
	//	m_CPUStep = 0;
	//}
	return 0;
}

void nes::CPUTick()
{
	//++m_CPUCycles;
	for (u32 i = 0; i < 3; ++i) { m_PPUFrame |= m_PictureProcessingUnit.EmulateCycle(m_Bus); }
}

void nes::Run()
{
	m_Cartridge.LoadCartridge("./carts/logo.nes");
	if (!m_Cartridge.m_Loaded) { return; }
	m_Processor.Reset(m_Bus);

	SDL_Texture* NESRender = SDL_CreateTexture(m_Context, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
	SDL_SetTextureBlendMode(NESRender, SDL_BLENDMODE_NONE);

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
			case SDL_WINDOWEVENT:
				break;
			}
		}

		Clock();

		if (m_PPUFrame)
		{
			SDL_RenderClear(m_Context);
			u32* pixels = nullptr; int pitch;
			SDL_LockTexture(NESRender, 0, (void**)&pixels, &pitch);
			for (u32 i = 0; i < 256 * 240; ++i)
			{
				pixels[i] = (m_PictureProcessingUnit.m_Screen[i] << 8) | 0xFF;
			}
			SDL_UnlockTexture(NESRender);
			SDL_RenderCopy(m_Context, NESRender, 0, 0);
			SDL_RenderPresent(m_Context);
			m_PPUFrame = false;
		}

	}

	SDL_DestroyTexture(NESRender);
}